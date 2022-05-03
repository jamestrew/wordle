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

void getPlayWord(char *testWord);

WINDOW *create_newwin(int height, int width, int starty, int startx,
                      char text) {
  // allocated window in mem
  WINDOW *local_win = newwin(height, width, starty, startx);

  box(local_win, 0, 0); // draws box
  mvwprintw(local_win, height / 2, width / 2, "%c", text);
  wrefresh(local_win);
  return local_win;
}

int main() {
  char playWord[WORD_LENGTH + 2]; // newline and null
  int ch;
  getPlayWord(playWord);

  initscr();
  raw();
  noecho();
  keypad(stdscr, TRUE);
  start_color();
  cbreak();
  curs_set(0);
  init_pair(1, COLOR_CYAN, COLOR_BLACK);
  init_pair(2, COLOR_RED, COLOR_BLACK);
  refresh();

  WINDOW *game_win = newwin(BOARD_HEIGHT, BOARD_WIDTH, 0, 0);
  keypad(game_win, TRUE);
  box(game_win, 0, 0);
  wattron(game_win, COLOR_PAIR(1));
  mvwprintw(game_win, 1, 1, "ENTER GUESS:");
  wattroff(game_win, COLOR_PAIR(1));

  mvwprintw(game_win, 2, 1, "> %c %c %c %c %c", playWord[0], playWord[1],
            playWord[2], playWord[3], playWord[4]);

  wattron(game_win, COLOR_PAIR(2));
  mvwprintw(game_win, BOARD_HEIGHT - 2, 1, "Press F1 to quit");
  wattroff(game_win, COLOR_PAIR(2));
  wrefresh(game_win);


  while ((ch = getch()) != KEY_F(1)) {
    printw("%c", ch);
  }

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
