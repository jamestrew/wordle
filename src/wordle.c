#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "wordle.h"

int main() {
  char playWord[WORD_LENGTH + 2]; // newline and null
  chtype ch;
  getPlayWord(playWord);

  initscr();
  WINDOW *game_win = initBoard();

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
      handleEnter(game_win);
      break;
    default:
      handleLetters(game_win, ch);
    }

#ifdef DEBUG
    debugCursor(game_win, ch);
#endif
    wrefresh(game_win);
  } while ((ch = getch()) != KEY_F(1));

  delwin(game_win);
  endwin();
}

void getPlayWord(char *playWord) {
  srand(time(0));
  int wordLine = rand() % WORD_COUNT;

  FILE *fp;
  fp = fopen("wordlist", "r");
  if (fp == NULL) {
    perror("Error opening file");
    exit(-1);
  }

  int line = 0;
  while (fgets(playWord, WORD_LENGTH + 2, fp) != NULL) {
    if (line >= wordLine)
      break;
    line++;
  }
  playWord[WORD_LENGTH] = '\0';
  fclose(fp);
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

void handleEnter(WINDOW *game_win) {
  int x, y;
  chtype ch;
  getyx(game_win, y, x);
  if (mvwinch(game_win, y, CONFIRM_START) != 'p')
    return;
  if (y == END_ROW) {
    gameEnd(game_win); // TODO
    return;
  }
  colorLetters(game_win); // TODO
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

void colorLetters(WINDOW *game_win) {}
void gameEnd(WINDOW *game_win) {}

void debugCursor(WINDOW *game_win, chtype ch) {
  int x, y;
  getyx(game_win, y, x);
  // TODO: kinda wanna add some padding - rending kinda broken
  mvwprintw(game_win, BOARD_HEIGHT - 2, BOARD_WIDTH - 7, "(%c)%d,%d", ch, x, y);
  wmove(game_win, y, x);
}
