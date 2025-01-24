/**
 * @file game_data.h
 * @brief Header file containing enums and structures related to the user
 * actions and game state.
 *
 * This file defines the user actions enum and the structure used for managing
 * the game data, such as the game field, score, and level.
 */
#ifndef BRICK_GAME_H
#define BRICK_GAME_H

/**
 * @brief Enum representing the different user actions in the game.
 *
 * This enum defines the possible actions that a user can take during gameplay,
 * such as moving the tetromino or performing an action.
 */
typedef enum {
  Start,
  Pause,
  Terminate,
  Left,
  Right,
  Up,
  Down,
  Action
} UserAction_t;

/**
 * @brief Structure containing game-related information.
 *
 * This structure holds data regarding the game state, including the game field,
 * the upcoming tetromino, the score, and the current game settings.
 */
typedef struct {
  int **field;
  int **next;
  int score;
  int high_score;
  int level;
  int speed;
  int pause;
} GameInfo_t;

GameInfo_t updateCurrentState();
void userInput(UserAction_t action, bool hold);

#endif