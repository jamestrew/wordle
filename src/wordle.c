#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define WORD_COUNT 659
#define WORD_LENGTH 5

#define BOARD_HEIGHT 25
#define BOARD_WIDTH 42

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
    │ ENTER GUESS:                          │
    │                                       │
    │ > A A A A A   press enter to confirm  │
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
int playRound(int roundNumber, const char *const playWord);

int main() {
  char playWord[WORD_LENGTH + 2]; // newline and null
  int ch;
  getPlayWord(playWord);

  initscr();
  WINDOW *game_win = initBoard();

  while ((ch = getch()) != KEY_F(1)) {

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
  }

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

  mvwprintw(local_win, START_ROW, START_COL, "> ");
  wrefresh(local_win);
  return local_win;
}
