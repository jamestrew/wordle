#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define WORD_COUNT 659
#define WORD_LENGTH 5

#define GUESS_COUNT 6
#define X_SPACING 2
#define Y_SPACING 2

#define BOARD_HEIGHT 25
#define BOARD_WIDTH 42

#define START_COL 3
#define START_ROW 3
#define END_COL START_COL + (WORD_LENGTH - 1) * X_SPACING
#define END_ROW START_ROW + (GUESS_COUNT - 1) * Y_SPACING

/* TODO:
   [x] get random word
   [ ] create basic play board
   [ ] create input handling
   [ ] do the letter coloring
   [ ] do the wordle colorful thing

   - 6 letters
   - green for correct guesses
   - yellow for correct but wrong positions
   - repeats aren't additinally yellow

   - black bg
    ┌───────────────────────────────────────┐
    │                                       │
    │ENTER GUESS:                           │
    │                                       │
    │> A A A A A   press enter to confirm   │
    │                                       │
    │                                       │
    │                                       │
    │                                       │
    │                                       │
    │                                       │
    │                                       │
    │                                       │
    │                                       │
    │                                       │
    │                                       │
    │                                       │
    │                                       │
    │                                       │
    │                                       │
    └───────────────────────────────────────┘
*/

void getPlayWord(char *playWord);
WINDOW *initBoard();
void handleArrows(WINDOW *game_win, int direction);
void handleEnter(WINDOW *game_win);
void handleBackspace(WINDOW *game_win);
void handleLetters(WINDOW *game_win, int ch);
void debugCursoryx(WINDOW *game_win);

int main() {
  char playWord[WORD_LENGTH + 2]; // newline and null
  int ch;
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
    debugCursoryx(game_win);
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

void handleArrows(WINDOW *game_win, int direction) {
  int x, y;
  getyx(game_win, y, x);

  if (direction == KEY_RIGHT && x < END_COL)
    wmove(game_win, y, x + 2);
  else if (direction == KEY_LEFT && x > START_COL)
    wmove(game_win, y, x - 2);
}

void handleEnter(WINDOW *game_win) {}

void handleBackspace(WINDOW *game_win) {}

void handleLetters(WINDOW *game_win, int ch) {}

void debugCursoryx(WINDOW *game_win) {
  int x, y;
  getyx(game_win, y, x);
  // TODO: kinda wanna add some padding - rending kinda broken
  mvwprintw(game_win, BOARD_HEIGHT - 2, BOARD_WIDTH - 5, "%d,%d", x, y);
  wmove(game_win, y, x);
}
