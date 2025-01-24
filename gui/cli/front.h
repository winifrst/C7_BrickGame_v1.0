/**
 * @file front.h
 * @brief Frontend interface for the Tetris game, managing the graphical
 * interface and user input.
 *
 * This header file defines the necessary structures and functions for managing
 * the graphical interface using ncurses and handling user input. It includes
 * function declarations for the game loop, rendering the game field, and user
 * interaction.
 *
 * It also includes definitions for various constants such as field size,
 * control keys, and user actions.
 */
#ifndef FRONT_H
#define FRONT_H

#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "../../brick_game/brick_game.h"

#define FIELD_WIDTH 10
#define FIELD_HEIGHT 20
#define TETR_SIZE 5
#define SPACE_KEY ' '
#define ENTER_KEY 10
#define EXIT_GAME -1

/**
 * @brief Represents the interface windows for the game.
 *
 * This structure contains the ncurses windows that are used for displaying
 * various parts of the game UI.
 */
typedef struct {
  WINDOW *game_win;
  WINDOW *next_win;
  WINDOW *info_win;
} Interface_t;

bool run_game_loop();
void print_field(const GameInfo_t *gameInfo, Interface_t *windows);
void print_next(const GameInfo_t *gameInfo, Interface_t *windows);
void print_info(const GameInfo_t *gameInfo, Interface_t *windows);
void free_game_info(GameInfo_t *gameInfo);
UserAction_t get_action(int key);
int offset_counter(int number);

#endif
