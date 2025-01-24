/**
 * @file move_logic.c
 * @brief Move logic for Tetris.
 */
#include "backend.h"

/**
 * @brief Moves the current tetromino based on the user input.
 *
 * This function processes user input to move the tetromino left, right, down,
 * or rotate, and performs actions based on those inputs.
 *
 * @param actual_info A pointer to the ModelInfo_t structure holding the game
 * state.
 */
void move_tetromino(ModelInfo_t *actual_info) {
  if (actual_info->hold) {
    switch (actual_info->user_action) {
      case Left:
        move_left(actual_info);
        break;
      case Right:
        move_right(actual_info);
        break;
      case Down:
        actual_info->state = Shifting;
        break;
      case Action:
        if (!is_rotation_blocked()) {
          copy_matrix(actual_info->current_tetromino,
                      actual_info->collision_test_tetromino, TETR_SIZE,
                      TETR_SIZE);
        }
        break;
      case Terminate:
      case Pause:
        actual_info->state = Pause_state;
        actual_info->pause = 1;
        break;
      default:
        break;
    }
  }
  if ((update_timer() - actual_info->timer) >=
      (700 - (actual_info->speed * 52))) {
    actual_info->timer = update_timer();
    actual_info->state = Shifting;
  }
}

/**
 * @brief Moves the current tetromino to the left.
 *
 * This function checks whether the tetromino can be moved left and moves it
 * if possible.
 *
 * @param actual_info A pointer to the ModelInfo_t structure holding the game
 * state.
 */
void move_left(ModelInfo_t *actual_info) {
  actual_info->x_position--;
  if (is_move_collision(actual_info)) actual_info->x_position++;
}

/**
 * @brief Moves the current tetromino to the right.
 *
 * This function checks whether the tetromino can be moved right and moves it
 * if possible.
 *
 * @param actual_info A pointer to the ModelInfo_t structure holding the game
 * state.
 */
void move_right(ModelInfo_t *actual_info) {
  actual_info->x_position++;
  if (is_move_collision(actual_info)) actual_info->x_position--;
}

/**
 * @brief Checks if a rotation is blocked for the current tetromino.
 *
 * This function checks if rotating the current tetromino will cause a collision
 * and returns an appropriate error code.
 *
 * @return Returns the error code indicating whether rotation is blocked or not.
 */
int is_rotation_blocked() {
  ModelInfo_t *actual_info = get_info();
  int error = NO_COLLISION;
  copy_matrix(actual_info->collision_test_tetromino,
              actual_info->current_tetromino, TETR_SIZE, TETR_SIZE);
  rotate(actual_info->current_type, &(actual_info->collision_test_tetromino));
  error = check_rotate_collision(actual_info);

  int origin_x_position = actual_info->x_position;

  if (error == RIGHT_COLLISION && error != BASE_COLLISION) {
    actual_info->x_position--;
    error = check_rotate_collision(actual_info);
  }

  for (int counter = 2; error == LEFT_COLLISION && counter > 0; counter--) {
    if (error != BASE_COLLISION) {
      actual_info->x_position++;
      error = check_rotate_collision(actual_info);
    }
  }

  if (error == BASE_COLLISION) actual_info->x_position++;
  error = check_rotate_collision(actual_info);
  if (error == BASE_COLLISION) actual_info->x_position -= 2;
  error = check_rotate_collision(actual_info);
  if (error == BASE_COLLISION) actual_info->x_position += 3;
  error = check_rotate_collision(actual_info);

  if (error) actual_info->x_position = origin_x_position;

  return error;
}

/**
 * @brief Rotates the current tetromino based on its type.
 *        Uses either a left or right rotation depending on the type.
 *
 * @param type The type of tetromino to rotate.
 * @param straight A pointer to the current tetromino's matrix.
 */
void rotate(TetrominoType_t type, int ***straight) {
  switch (type) {
    case O_tetromino:
      break;
    case I_tetromino:
    case S_tetromino:
    case Z_tetromino:
      (*straight)[1][2] ? rotate_left(straight) : rotate_right(straight);
      break;
    case T_tetromino:
    case L_tetromino:
    case J_tetromino:
      rotate_right(straight);
      break;
  }
}

/**
 * @brief Rotates the tetromino counterclockwise (left).
 *
 * @param straight A pointer to the matrix representing the current tetromino.
 */
void rotate_left(int ***straight) {
  int **rotated = NULL;
  int error = create_matrix(&rotated, TETR_SIZE, TETR_SIZE);
  if (!error) {
    for (int i = 0; i < TETR_SIZE; i++) {
      for (int j = 0; j < TETR_SIZE; j++) {
        rotated[TETR_SIZE - 1 - j][i] = (*straight)[i][j];
      }
    }
    copy_matrix(*straight, rotated, TETR_SIZE, TETR_SIZE);
    remove_matrix(&rotated, TETR_SIZE);
  }
}

/**
 * @brief Rotates the tetromino clockwise (right).
 *
 * @param straight A pointer to the matrix representing the current tetromino.
 */
void rotate_right(int ***straight) {
  int **rotated = NULL;
  int error = create_matrix(&rotated, TETR_SIZE, TETR_SIZE);
  if (!error) {
    for (int i = 0; i < TETR_SIZE; i++) {
      for (int j = 0; j < TETR_SIZE; j++) {
        rotated[j][TETR_SIZE - 1 - i] = (*straight)[i][j];
      }
    }
    copy_matrix(*straight, rotated, TETR_SIZE, TETR_SIZE);
    remove_matrix(&rotated, TETR_SIZE);
  }
}

/**
 * @brief Moves the current tetromino down if it doesn't collide with the base
 * or floor.
 *
 * @param actual_info A pointer to the game model information.
 */
void shift_tetromino(ModelInfo_t *actual_info) {
  actual_info->y_position++;
  if (is_move_collision(actual_info)) {
    actual_info->y_position--;
    actual_info->state = Attaching;
  } else
    actual_info->state = Moving;
}

/**
 * @brief Attaches the current tetromino to the game field and checks for full
 * lines.
 *
 * @param actual_info A pointer to the game model information.
 */
void attach_tetromino(ModelInfo_t *actual_info) {
  set_tetromino_on_field(actual_info->field_base, actual_info);
  calculate_lines(actual_info);
  actual_info->state = Spawn;
}

/**
 * @brief Checks if the tetromino can move sideways or downward without
 * collisions.
 *
 * This function checks if the current tetromino can be moved by
 * analyzing possible collisions with the game field boundaries and other
 * tetrominos. Each block of the tetromino is checked for movement.
 *
 * @param actual_info Pointer to the structure containing the current tetromino
 * and game field information.
 * @return Error code
 */
int is_move_collision(const ModelInfo_t *actual_info) {
  int error = NO_COLLISION;

  for (int i = 0; i < TETR_SIZE; i++) {
    for (int j = 0; j < TETR_SIZE; j++) {
      int x_offset = actual_info->x_position + j;
      int y_offset = actual_info->y_position + i;

      if (actual_info->current_tetromino[i][j] != 0 && !error) {
        if (y_offset >= FIELD_HEIGHT) {
          error = FLOOR_COLLISION;
        } else if (x_offset < 0) {
          error = BASE_COLLISION;
        } else if (y_offset >= 0 &&
                   actual_info->field_base[y_offset][x_offset] != 0) {
          error = BASE_COLLISION;
        }
      }
    }
  }
  return error;
}

/**
 * @brief Checks if the tetromino can rotate.
 *
 * This function checks if the tetromino can be rotated by analyzing possible
 * collisions with other blocks or field boundaries. All blocks of the rotated
 * tetromino are checked for collision.
 *
 * @param actual_info Pointer to the structure containing the current tetromino
 * and game field information.
 * @return Error code:
 *         - `NO_COLLISION` (0) — rotation is possible.
 *         - `FLOOR_COLLISION` — collision with the floor (below the tetromino).
 *         - `LEFT_COLLISION` — collision with the left boundary.
 *         - `RIGHT_COLLISION` — collision with the right boundary.
 */
int check_rotate_collision(const ModelInfo_t *actual_info) {
  int error = NO_COLLISION;
  for (int i = 0; i < TETR_SIZE; i++) {
    for (int j = 0; j < TETR_SIZE; j++) {
      int x_offset = actual_info->x_position + j;
      int y_offset = actual_info->y_position + i;

      if (y_offset >= FIELD_HEIGHT &&
          actual_info->collision_test_tetromino[i][j] != 0) {
        error = FLOOR_COLLISION;
      } else if (actual_info->collision_test_tetromino[i][j] != 0) {
        if (x_offset < 0) {
          error = LEFT_COLLISION;
        } else if (x_offset >= FIELD_WIDTH) {
          error = RIGHT_COLLISION;
        } else if (y_offset >= 0 &&
                   actual_info->field_base[y_offset][x_offset] != 0)
          error = BASE_COLLISION;
      }
    }
  }
  return error;
}
