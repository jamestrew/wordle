#ifndef WORDLE

#include <ncurses.h>

#define WORD_COUNT 659
#define WORD_LENGTH 5
#define S_WORD_LEN WORD_LENGTH + 1

#define GUESS_COUNT 6
#define X_SPACING 2
#define Y_SPACING 2

#define BOARD_HEIGHT 25
#define BOARD_WIDTH 42

#define START_COL 3
#define START_ROW 3
#define POINTER_COL START_COL - 2
#define POINTER ">"
#define END_COL START_COL + (WORD_LENGTH - 1) * X_SPACING
#define END_ROW START_ROW + (GUESS_COUNT - 1) * Y_SPACING

#define CONFIRM_START END_COL + 5
#define CONFIRM_MSG "press <enter> to confirm"
#define INVALID_MSG "invalid guess, try again"

#define C_INTRO 1
#define C_QUIT 2
#define C_CORRECT 3
#define C_CLOSE 4
#define C_WRONG 5
#define COLOR_GREY 69

typedef enum {
  CORRECT = C_CORRECT,
  CLOSE = C_CLOSE,
  WRONG = C_WRONG,
} guess_t;

typedef struct {
  char playWord[S_WORD_LEN];
  char allWords[WORD_COUNT][S_WORD_LEN];
  char currGuess[S_WORD_LEN];
  guess_t guessColors[GUESS_COUNT][S_WORD_LEN];
  int guessCount;
} GameData;

GameData *initGame();
WINDOW *initBoard();

// input handlers
void handleArrows(WINDOW *game_win, chtype direction);
void handleEnter(WINDOW *game_win, GameData *gameData);
void handleBackspace(WINDOW *game_win);
void handleLetters(WINDOW *game_win, chtype ch);
void clearConfirmMsg(WINDOW *game_win, int y);

void getGuess(WINDOW *game_win, char *guess);
bool isValidGuess(GameData *gameData);

// wordle logic
void colorLetters(WINDOW *game_win, GameData *gameData);
void gameEnd(WINDOW *game_win);

// dev tools
void debugCursor(WINDOW *game_win, chtype ch, char *playWord);
void debugGuesses(WINDOW *game_win, GameData *gameData);

#endif
