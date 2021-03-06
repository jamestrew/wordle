#include <locale.h>
#include <ncurses.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "wordle.h"

int main() {
  setlocale(LC_CTYPE, "");
  initscr();
  GameData *gameData = initGame();
  WINDOW *game_win = initBoard();

  chtype ch;
  do {
    switch (ch) {
    case KEY_LEFT:
      handleArrows(game_win, KEY_LEFT);
      break;
    case KEY_RIGHT:
      handleArrows(game_win, KEY_RIGHT);
      break;
    case KEY_BACKSPACE:
      handleBackspace(game_win);
      break;
    case 10:
      handleEnter(game_win, gameData);
      break;
    default:
      handleLetters(game_win, ch);
    }

#ifdef DEBUG
    debugCursor(game_win, ch, gameData->playWord);
    debugGuesses(game_win, gameData);
#endif
    wrefresh(game_win);
  } while ((ch = getch()) != KEY_F(1));

  free(gameData);
  delwin(game_win);
  endwin();
}

GameData *initGame() {
  GameData *gameData = malloc(sizeof(GameData));
  if (gameData == NULL) {
    perror("Error allocating game data memory");
    exit(-1);
  }
  gameData->guessCount = 0;
  for (int i = 0; i < GUESS_COUNT; ++i)
    for (int j = 0; j < WORD_LENGTH; ++j)
      gameData->guessColors[i][j] = WRONG;

  srand(time(0));
  int wordLine = rand() % WORD_COUNT;

  FILE *fp;
  fp = fopen("wordlist", "r");
  if (fp == NULL) {
    perror("Error opening file");
    exit(-1);
  }

  int line = 0;
  char word[WORD_LENGTH + 2];
  while (fgets(word, WORD_LENGTH + 2, fp) != NULL || line <= WORD_COUNT) {
    word[WORD_LENGTH] = '\0';
    strcpy(gameData->allWords[line], word);
    line++;
  }
  fclose(fp);
  strcpy(gameData->playWord, gameData->allWords[wordLine]);

  return gameData;
}

WINDOW *initBoard() {
  noecho();
  keypad(stdscr, TRUE);
  start_color();
  cbreak();
  init_color(COLOR_GREY, 134, 145, 137);
  init_pair(C_INTRO, COLOR_CYAN, COLOR_BLACK);
  init_pair(C_QUIT, COLOR_RED, COLOR_BLACK);
  init_pair(C_CORRECT, COLOR_GREEN, COLOR_BLACK);
  init_pair(C_CLOSE, COLOR_YELLOW, COLOR_BLACK);
  init_pair(C_WRONG, COLOR_GREY, COLOR_BLACK);
  refresh();

  WINDOW *local_win = newwin(BOARD_HEIGHT, BOARD_WIDTH, 0, 0);
  box(local_win, 0, 0);

  wattron(local_win, COLOR_PAIR(C_INTRO));
  mvwprintw(local_win, 1, 1, "ENTER GUESS:");
  wattroff(local_win, COLOR_PAIR(C_INTRO));

  wattron(local_win, COLOR_PAIR(C_QUIT));
  mvwprintw(local_win, BOARD_HEIGHT - 2, 1, "Press F1 to quit");
  wattroff(local_win, COLOR_PAIR(C_QUIT));

  mvwprintw(local_win, START_ROW, POINTER_COL, POINTER);
  wmove(local_win, START_ROW, START_COL);
  wrefresh(local_win);
  return local_win;
}

void handleArrows(WINDOW *game_win, chtype direction) {
  int x, y;
  getyx(game_win, y, x);

  if (direction == KEY_RIGHT && x < END_COL)
    wmove(game_win, y, x + X_SPACING);
  else if (direction == KEY_LEFT && x > START_COL)
    wmove(game_win, y, x - X_SPACING);
}

void handleEnter(WINDOW *game_win, GameData *gameData) {
  int x, y;
  getyx(game_win, y, x);
  if (mvwinch(game_win, y, CONFIRM_START) != 'p') {
    wmove(game_win, y, x);
    return;
  }

  getGuess(game_win, gameData->currGuess);
  if (!isValidGuess(gameData)) {
    clearConfirmMsg(game_win, y);
    mvwprintw(game_win, y, CONFIRM_START, INVALID_MSG);
    wmove(game_win, y, x);
    return;
  }

  colorLetters(game_win, gameData);
  clearConfirmMsg(game_win, y);
  if (y == END_ROW || strcmp(gameData->currGuess, gameData->playWord) == 0) {
    gameEnd(game_win, gameData);
    return;
  }
  mvwprintw(game_win, y, POINTER_COL, " ");
  mvwprintw(game_win, y + Y_SPACING, POINTER_COL, POINTER);
  wmove(game_win, y + Y_SPACING, START_COL);
  gameData->guessCount++;
}

void handleBackspace(WINDOW *game_win) {
  int x, y;

  getyx(game_win, y, x);
  if (x <= START_COL)
    return;

  if (winch(game_win) == ' ')
    x -= X_SPACING;
  mvwprintw(game_win, y, x, " ");
  clearConfirmMsg(game_win, y);
  wmove(game_win, y, x);
}

void handleLetters(WINDOW *game_win, chtype ch) {
  if (ch >= 65 && ch <= 90) // A-Z
    ch -= 32;
  if (!(ch >= 97) || !(ch <= 122)) // not a-z
    return;

  wprintw(game_win, "%c ", ch);

  int x, y;
  getyx(game_win, y, x);
  if (x == END_COL + X_SPACING) {
    mvwprintw(game_win, y, CONFIRM_START, CONFIRM_MSG);
    wmove(game_win, y, END_COL);
  }
}

void clearConfirmMsg(WINDOW *game_win, int y) {
  char *clearMsg = malloc(strlen(CONFIRM_MSG) + 1);
  memset(clearMsg, ' ', strlen(CONFIRM_MSG));
  mvwprintw(game_win, y, CONFIRM_START, "%s", clearMsg);
  free(clearMsg);
}

void getGuess(WINDOW *game_win, char *guess) {
  int x, y;
  getyx(game_win, y, x);
  for (int col = START_COL, idx = 0; col <= END_COL; col += 2, idx++) {
    int ch = mvwinch(game_win, y, col);
    guess[idx] = ch;
  }
}

bool isValidGuess(GameData *gameData) {
  int left = 0, right = WORD_COUNT, mid;
  int cmp;

  while (left <= right) {
    mid = (right - left) / 2 + left;
    cmp = strcmp(gameData->currGuess, gameData->allWords[mid]);
    if (cmp < 0) {
      right = mid - 1;
    } else if (cmp > 0) {
      left = mid + 1;
    } else {
      return true;
    }
  }
  return false;
}

void colorLetters(WINDOW *game_win, GameData *gameData) {
  // FIX: could maybe optimize
  int x, y;
  getyx(game_win, y, x);

  char letters[S_WORD_LEN];
  strcpy(letters, gameData->playWord);

  for (int col = START_COL, idx = 0; col <= END_COL; col += 2, idx++) {
    chtype ch = mvwinch(game_win, y, col);
    if (gameData->playWord[idx] == ch) {
      letters[idx] = 0;
      wattron(game_win, COLOR_PAIR(C_CORRECT));
      mvwprintw(game_win, y, col, "%c", ch);
      wattroff(game_win, COLOR_PAIR(C_CORRECT));
      gameData->guessColors[gameData->guessCount][idx] = C_CORRECT;
    }
  }
  for (int col = START_COL, idx = 0; col <= END_COL; col += 2, idx++) {
    chtype ch = mvwinch(game_win, y, col);
    if (gameData->playWord[idx] != ch) {
      for (int i = 0; i < WORD_LENGTH; ++i) {
        if (ch == letters[i]) {
          letters[i] = 0;
          wattron(game_win, COLOR_PAIR(C_CLOSE));
          mvwprintw(game_win, y, col, "%c", ch);
          wattroff(game_win, COLOR_PAIR(C_CLOSE));
          gameData->guessColors[gameData->guessCount][idx] = C_CLOSE;
        }
      }
    }
  }
}

void gameEnd(WINDOW *game_win, GameData *gameData) {
  int yOffset = 0;
  for (int row = 0; row <= gameData->guessCount; ++row) {
    int xOffset = 0;
    for (int col = 0; col < WORD_LENGTH; ++col) {
      int color = gameData->guessColors[row][col];
      wattron(game_win, COLOR_PAIR(color));
      mvwprintw(game_win, END_MSG_ROW + yOffset, col + xOffset + 3, "%s", "??????");
      wattroff(game_win, COLOR_PAIR(color));
      xOffset++;
    }
    yOffset++;
  }

  int x, y;
  getyx(game_win, y, x);

  if (strcmp(gameData->playWord, gameData->currGuess) == 0) {
    mvwprintw(game_win, y + 1, 3, "Guessed in %d tries", ++gameData->guessCount);
  } else {
    mvwprintw(game_win, y + 1, 3, "\n   You lose. The word was: %s", gameData->playWord);
  }
}

void debugCursor(WINDOW *game_win, chtype ch, char *playWord) {
  int x, y;
  getyx(game_win, y, x);
  char input[5];
  if (ch == 10)
    strcpy(input, "<CR>");
  else
    sprintf(input, "%c", ch);

  mvprintw(BOARD_HEIGHT + 2, 0, "%s", playWord);
  mvprintw(BOARD_HEIGHT + 3, 0, "(%s - %d) (%d,%d)", input, ch, x, y);
  wmove(game_win, y, x);
}

void debugGuesses(WINDOW *game_win, GameData *gameData) {
  int x, y;
  getyx(game_win, y, x);

  int offset = 0;
  for (int row = 0; row < GUESS_COUNT; ++row) {
    int coloffset = 0;
    for (int col = 0; col < WORD_LENGTH; ++col) {
      int color = gameData->guessColors[row][col];
      mvprintw(row + 30 + offset, col + coloffset + 3, "%d", color);
      coloffset++;
    }
    offset++;
  }
  wmove(game_win, y, x);
}
