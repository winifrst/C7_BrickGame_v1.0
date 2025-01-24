/**
 * @file backend.h
 * @brief Header file for the backend logic of the Tetris game.
 *
 * This file defines constants, types, and structures used for managing the game
 * state and logic, including the tetromino types, field dimensions, and
 * game-related structures.
 */
#ifndef BACKEND_H
#define BACKEND_H

#include <ncurses.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>

#include "../brick_game.h"

#define FIELD_WIDTH 10
#define FIELD_HEIGHT 20
#define TETR_SIZE 5

#define EXIT_GAME -1

#define SPAWN_X_POSITION 3
#define SPAWN_Y_POSITION -2

#define NO_COLLISION 0
#define BASE_COLLISION 1
#define FLOOR_COLLISION 2
#define LEFT_COLLISION 3
#define RIGHT_COLLISION 4
#define LEFT_BASE_COLLISION 5
#define RIGHT_BASE_COLLISION 6

/**
 * @brief Enum representing the different types of tetrominos.
 */
typedef enum {
  O_tetromino = 1,
  I_tetromino,
  T_tetromino,
  S_tetromino,
  Z_tetromino,
  J_tetromino,
  L_tetromino
} TetrominoType_t;

/**
 * @brief Enum representing the possible game states.
 *
 * These states manage the progression of the game, including spawning
 * tetrominos, moving them, attaching them to the field, pausing, and the game
 * over state.
 */
typedef enum {
  Start_state,
  Spawn,
  Moving,
  Shifting,
  Attaching,
  Game_over,
  Pause_state,
  Exit_state
} FiniteState_t;

/**
 * @brief Structure containing all necessary information about the current game
 * model.
 *
 * This structure holds information about the game state, including the current
 * tetromino, game field, score, level, and more.
 */
typedef struct {
  FiniteState_t state;
  UserAction_t user_action;
  bool hold;
  int **field_base;
  int **next_tetromino;
  TetrominoType_t next_type;
  int **current_tetromino;
  int **collision_test_tetromino;
  TetrominoType_t current_type;
  int x_position;
  int y_position;
  int score;
  int high_score;
  int level;
  int speed;
  int pause;
  long long int timer;
} ModelInfo_t;

ModelInfo_t *get_info();
void run_actions_by_state(ModelInfo_t *actual_info);
void initialize_game(ModelInfo_t *actual_info);
void spawn_tetromino(ModelInfo_t *actual_info);
void calculate_lines(ModelInfo_t *actual_info);
void clear_line(int **field_base, int line);
void update_score(int *score, int lines_cleared);
void update_speed_and_level(ModelInfo_t *actual_info);
void pause_actions(ModelInfo_t *actual_info);
void game_over_actions(ModelInfo_t *actual_info);
void run_terminate_actions(ModelInfo_t *actual_info);
TetrominoType_t generate_next_tetromino(int **next);
void fill_tetromino(int **filled, TetrominoType_t num);
long long int update_timer();

int create_matrix(int ***matrix, int rows, int columns);
void remove_matrix(int ***matrix, int rows);
void free_result(GameInfo_t *result);
void copy_matrix(int **dest, int **src, int rows, int columns);
void reset_matrix(int **src, int rows, int columns);
void set_tetromino_on_field(int **field, ModelInfo_t *actual_info);
void write_score(int high_score);
int read_score();

void move_tetromino(ModelInfo_t *actual_info);
void move_left(ModelInfo_t *actual_info);
void move_right(ModelInfo_t *actual_info);
int is_rotation_blocked();
void rotate(TetrominoType_t type, int ***straight);
void rotate_left(int ***straight);
void rotate_right(int ***straight);
void shift_tetromino(ModelInfo_t *actual_info);
void attach_tetromino(ModelInfo_t *actual_info);
int is_move_collision(const ModelInfo_t *actual_info);
int check_rotate_collision(const ModelInfo_t *actual_info);

#endif