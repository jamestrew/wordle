#ifndef WORDLE

#include <ncurses.h>

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

#define CONFIRM_START END_COL + 5
#define CONFIRM_MSG "press <enter> to confirm"

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

// input handlers
void handleArrows(WINDOW *game_win, chtype direction);
void handleEnter(WINDOW *game_win);
void handleBackspace(WINDOW *game_win);
void handleLetters(WINDOW *game_win, chtype ch);

// wordle logic
void colorLetters(WINDOW *game_win);

// dev tools
void debugCursor(WINDOW *game_win, chtype ch);

#endif
