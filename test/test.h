#ifndef TEST_H
#define TEST_H

#include <check.h>
#include <stdbool.h>
#include <stdio.h>

#include "../brick_game/brick_game.h"
#include "../brick_game/tetris/backend.h"

#define SUCCESS 1
#define FAILURE 0

Suite *tetris(void);

#endif