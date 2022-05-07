#include <ncurses.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "wordle.h"

int main() {
  char words[WORD_COUNT][S_WORD_LEN];
  char playWord[S_WORD_LEN];
  getPlayWord(words, playWord);

  initscr();
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
      handleEnter(game_win, playWord, words);
      break;
    default:
      handleLetters(game_win, ch);
    }

#ifdef DEBUG
    debugCursor(game_win, ch, playWord);
#endif
    wrefresh(game_win);
  } while ((ch = getch()) != KEY_F(1));

  delwin(game_win);
  endwin();
}

void getPlayWord(char words[][S_WORD_LEN], char *playWord) {
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
    strcpy(words[line], word);
    line++;
  }
  fclose(fp);
  strcpy(playWord, words[wordLine]);
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

void handleEnter(WINDOW *game_win, char *playWord, char words[][S_WORD_LEN]) {
  int x, y;
  chtype ch;
  getyx(game_win, y, x);
  if (mvwinch(game_win, y, CONFIRM_START) != 'p')
    return;
  if (y == END_ROW) {
    gameEnd(game_win); // TODO
    return;
  }

  char guess[S_WORD_LEN];
  getGuess(game_win, guess);
  if (!isValidGuess(words, guess)) {
    clearConfirmMsg(game_win, y);
    mvwprintw(game_win, y, CONFIRM_START, INVALID_MSG);
    wmove(game_win, y, x);
    return;
  }

  colorLetters(game_win, playWord);
  clearConfirmMsg(game_win, y);
  mvwprintw(game_win, y, POINTER_COL, " ");
  mvwprintw(game_win, y + Y_SPACING, POINTER_COL, POINTER);
  wmove(game_win, y + Y_SPACING, START_COL);
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

bool isValidGuess(char words[][S_WORD_LEN], char *guess) {
  int left = 0, right = WORD_COUNT, mid;
  int cmp;

  while (left <= right) {
    mid = (right - left) / 2 + left;
    cmp = strcmp(guess, words[mid]);
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

void colorLetters(WINDOW *game_win, char *playWord) {
  // FIX: could maybe optimize
  int x, y;
  getyx(game_win, y, x);

  char letters[S_WORD_LEN];
  strcpy(letters, playWord);

  for (int col = START_COL, idx = 0; col <= END_COL; col += 2, idx++) {
    chtype ch = mvwinch(game_win, y, col);
    if (playWord[idx] == ch) {
      letters[idx] = 0;
      wattron(game_win, COLOR_PAIR(C_CORRECT));
      mvwprintw(game_win, y, col, "%c", ch);
      wattroff(game_win, COLOR_PAIR(C_CORRECT));
    }
  }
  for (int col = START_COL, idx = 0; col <= END_COL; col += 2, idx++) {
    chtype ch = mvwinch(game_win, y, col);
    if (playWord[idx] != ch) {
      for (int i = 0; i < WORD_LENGTH; ++i) {
        if (ch == letters[i]) {
          letters[i] = 0;
          wattron(game_win, COLOR_PAIR(C_CLOSE));
          mvwprintw(game_win, y, col, "%c", ch);
          wattroff(game_win, COLOR_PAIR(C_CLOSE));
        }
      }
    }
  }
}

void gameEnd(WINDOW *game_win) {}

void debugCursor(WINDOW *game_win, chtype ch, char *playWord) {
  int x, y;
  getyx(game_win, y, x);
  // TODO: kinda wanna add some padding - rending kinda broken
  mvwprintw(game_win, BOARD_HEIGHT - 2, BOARD_WIDTH - 7, "(%c)%d,%d", ch, x, y);
  mvwprintw(game_win, 1, 13, " %s", playWord);
  wmove(game_win, y, x);
}
