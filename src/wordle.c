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
    case KEY_ENTER:
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
  // curs_set(0);
  init_pair(1, COLOR_CYAN, COLOR_BLACK);
  init_pair(2, COLOR_RED, COLOR_BLACK);
  refresh();

  WINDOW *local_win = newwin(BOARD_HEIGHT, BOARD_WIDTH, 0, 0);
  box(local_win, 0, 0);

  wattron(local_win, COLOR_PAIR(1));
  mvwprintw(local_win, 1, 1, "ENTER GUESS:");
  wattroff(local_win, COLOR_PAIR(1));

  wattron(local_win, COLOR_PAIR(2));
  mvwprintw(local_win, BOARD_HEIGHT - 2, 1, "Press F1 to quit");
  wattroff(local_win, COLOR_PAIR(2));

  mvwprintw(local_win, START_ROW, START_COL - 2, "> ");
  wrefresh(local_win);
  return local_win;
}

void handleArrows(WINDOW *game_win, chtype direction) {
  int x, y;
  getyx(game_win, y, x);

  if (direction == KEY_RIGHT && x < END_COL)
    wmove(game_win, y, x + 2);
  else if (direction == KEY_LEFT && x > START_COL)
    wmove(game_win, y, x - 2);
}

void handleEnter(WINDOW *game_win) {}

void handleBackspace(WINDOW *game_win) {
  // if letter under clear under cursor
  // else clear prev letter
  int x, y;
  chtype ch;

  getyx(game_win, y, x);
  ch = winch(game_win);
  if (ch == 0) {
    mvwprintw(game_win, y, x - 3, " ");
  } else {
    mvwprintw(game_win, y, x - 1, " ");
  }
}

void handleLetters(WINDOW *game_win, chtype ch) {
  if (ch >= 65 && ch <= 90) // A-Z
    ch -= 32;
  if (!(ch >= 97) || !(ch <= 122)) // not a-z
    return;

  wprintw(game_win, "%c ", ch);

  int x, y;
  getyx(game_win, y, x);
  if (x == END_COL + 2) {
    wprintw(game_win, "   press <enter> to confirm");
    wmove(game_win, y, END_COL);
  }
}

void debugCursor(WINDOW *game_win, chtype ch) {
  int x, y;
  getyx(game_win, y, x);
  // TODO: kinda wanna add some padding - rending kinda broken
  mvwprintw(game_win, BOARD_HEIGHT - 2, BOARD_WIDTH - 7, "(%c)%d,%d", ch, x, y);
  wmove(game_win, y, x);
}
