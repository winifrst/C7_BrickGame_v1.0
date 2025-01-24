/**
 * @file move_logic.c
 * @brief Move logic for Tetris.
 */
#include "backend.h"

/**
 * @brief Moves the current tetramino based on the user input.
 *
 * This function processes user input to move the tetramino left, right, down,
 * or rotate, and performs actions based on those inputs.
 *
 * @param actual_info A pointer to the ModelInfo_t structure holding the game
 * state.
 */
void move_tetramino(ModelInfo_t *actual_info) {
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
          copy_matrix(actual_info->current_tetramino,
                      actual_info->collision_test_tetramino, TETR_SIZE,
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
 * @brief Moves the current tetramino to the left.
 *
 * This function checks whether the tetramino can be moved left and moves it
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
 * @brief Moves the current tetramino to the right.
 *
 * This function checks whether the tetramino can be moved right and moves it
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
 * @brief Checks if a rotation is blocked for the current tetramino.
 *
 * This function checks if rotating the current tetramino will cause a collision
 * and returns an appropriate error code.
 *
 * @return Returns the error code indicating whether rotation is blocked or not.
 */
int is_rotation_blocked() {
  ModelInfo_t *actual_info = get_info();
  int error = NO_COLLISION;
  copy_matrix(actual_info->collision_test_tetramino,
              actual_info->current_tetramino, TETR_SIZE, TETR_SIZE);
  rotate(actual_info->current_type, &(actual_info->collision_test_tetramino));
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
 * @brief Rotates the current tetramino based on its type.
 *        Uses either a left or right rotation depending on the type.
 *
 * @param type The type of tetramino to rotate.
 * @param straight A pointer to the current tetramino's matrix.
 */
void rotate(TetraminoType_t type, int ***straight) {
  switch (type) {
    case O_tetramino:
      break;
    case I_tetramino:
    case S_tetramino:
    case Z_tetramino:
      (*straight)[1][2] ? rotate_left(straight) : rotate_right(straight);
      break;
    case T_tetramino:
    case L_tetramino:
    case J_tetramino:
      rotate_right(straight);
      break;
  }
}

/**
 * @brief Rotates the tetramino counterclockwise (left).
 *
 * @param straight A pointer to the matrix representing the current tetramino.
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
 * @brief Rotates the tetramino clockwise (right).
 *
 * @param straight A pointer to the matrix representing the current tetramino.
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
 * @brief Moves the current tetramino down if it doesn't collide with the base
 * or floor.
 *
 * @param actual_info A pointer to the game model information.
 */
void shift_tetramino(ModelInfo_t *actual_info) {
  actual_info->y_position++;
  if (is_move_collision(actual_info)) {
    actual_info->y_position--;
    actual_info->state = Attaching;
  } else
    actual_info->state = Moving;
}

/**
 * @brief Attaches the current tetramino to the game field and checks for full
 * lines.
 *
 * @param actual_info A pointer to the game model information.
 */
void attach_tetramino(ModelInfo_t *actual_info) {
  set_tetramino_on_field(actual_info->field_base, actual_info);
  calculate_lines(actual_info);
  actual_info->state = Spawn;
}

/**
 * @brief Checks if the tetramino can move sideways or downward without
 * collisions.
 *
 * This function checks if the current tetramino can be moved by
 * analyzing possible collisions with the game field boundaries and other
 * tetraminos. Each block of the tetramino is checked for movement.
 *
 * @param actual_info Pointer to the structure containing the current tetramino
 * and game field information.
 * @return Error code
 */
int is_move_collision(const ModelInfo_t *actual_info) {
  int error = NO_COLLISION;

  for (int i = 0; i < TETR_SIZE; i++) {
    for (int j = 0; j < TETR_SIZE; j++) {
      int x_offset = actual_info->x_position + j;
      int y_offset = actual_info->y_position + i;

      if (actual_info->current_tetramino[i][j] != 0 && !error) {
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
 * @brief Checks if the tetramino can rotate.
 *
 * This function checks if the tetramino can be rotated by analyzing possible
 * collisions with other blocks or field boundaries. All blocks of the rotated
 * tetramino are checked for collision.
 *
 * @param actual_info Pointer to the structure containing the current tetramino
 * and game field information.
 * @return Error code:
 *         - `NO_COLLISION` (0) — rotation is possible.
 *         - `FLOOR_COLLISION` — collision with the floor (below the tetramino).
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
          actual_info->collision_test_tetramino[i][j] != 0) {
        error = FLOOR_COLLISION;
      } else if (actual_info->collision_test_tetramino[i][j] != 0) {
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
