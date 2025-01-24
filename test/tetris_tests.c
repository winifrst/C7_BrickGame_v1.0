#include "test.h"

START_TEST(update_state) {
  GameInfo_t test = {NULL, NULL, 0, 0, 0, 0, 0};
  ck_assert_ptr_eq(test.field, NULL);
  ck_assert_ptr_eq(test.next, NULL);
  test = updateCurrentState();

  ck_assert_ptr_nonnull(test.field);
  ck_assert_ptr_nonnull(test.next);
  ck_assert_int_eq(test.score, 0);
  ck_assert_int_eq(test.high_score, read_score());
  ck_assert_int_eq(test.level, 1);
  ck_assert_int_eq(test.speed, 0);
  ck_assert_int_eq(test.pause, 0);

  remove_matrix(&test.field, FIELD_HEIGHT);
  remove_matrix(&test.next, TETR_SIZE);
}
END_TEST

START_TEST(get_model_info) {
  ModelInfo_t actual_info = {0};
  ck_assert_ptr_eq(actual_info.field_base, NULL);
  ck_assert_ptr_eq(actual_info.current_tetromino, NULL);
  ck_assert_ptr_eq(actual_info.next_tetromino, NULL);
  ck_assert_ptr_eq(actual_info.collision_test_tetromino, NULL);

  actual_info = *get_info();

  ck_assert_int_eq(actual_info.state, Start_state);
  ck_assert_ptr_nonnull(actual_info.field_base);
  ck_assert_ptr_nonnull(actual_info.current_tetromino);
  ck_assert_ptr_nonnull(actual_info.next_tetromino);
  ck_assert_ptr_nonnull(actual_info.collision_test_tetromino);
  ck_assert_int_eq(actual_info.x_position, SPAWN_X_POSITION);
  ck_assert_int_eq(actual_info.y_position, SPAWN_Y_POSITION);
  ck_assert_int_eq(actual_info.score, 0);
  ck_assert_int_eq(actual_info.high_score, read_score());
  ck_assert_int_eq(actual_info.level, 1);
  ck_assert_int_eq(actual_info.speed, 0);
  ck_assert_int_eq(actual_info.pause, 0);

  remove_matrix(&actual_info.field_base, FIELD_HEIGHT);
  remove_matrix(&actual_info.current_tetromino, TETR_SIZE);
  remove_matrix(&actual_info.next_tetromino, TETR_SIZE);
  remove_matrix(&actual_info.collision_test_tetromino, TETR_SIZE);
}
END_TEST

START_TEST(spawn) {
  ModelInfo_t *actual_info = (ModelInfo_t *)malloc(sizeof(ModelInfo_t));

  create_matrix(&actual_info->field_base, FIELD_HEIGHT, FIELD_WIDTH);
  create_matrix(&actual_info->next_tetromino, TETR_SIZE, TETR_SIZE);
  create_matrix(&actual_info->current_tetromino, TETR_SIZE, TETR_SIZE);
  create_matrix(&actual_info->collision_test_tetromino, TETR_SIZE, TETR_SIZE);
  actual_info->x_position = 0;
  actual_info->y_position = 0;

  for (TetrominoType_t i = O_tetromino; i <= L_tetromino; i++) {
    reset_matrix(actual_info->next_tetromino, TETR_SIZE, TETR_SIZE);
    fill_tetromino(actual_info->next_tetromino, i);
  }
  spawn_tetromino(actual_info);
  int **temp;
  create_matrix(&temp, TETR_SIZE, TETR_SIZE);
  fill_tetromino(temp, L_tetromino);

  for (int i = 0; i < TETR_SIZE; i++) {
    for (int j = 0; j < TETR_SIZE; j++) {
      ck_assert_int_eq(temp[i][j], actual_info->current_tetromino[i][j]);
    }
  }

  ck_assert_int_eq(actual_info->x_position, SPAWN_X_POSITION);
  ck_assert_int_eq(actual_info->y_position, SPAWN_Y_POSITION);

  ck_assert_int_eq(actual_info->state, Moving);

  remove_matrix(&(actual_info)->field_base, FIELD_HEIGHT);
  remove_matrix(&(actual_info)->current_tetromino, TETR_SIZE);
  remove_matrix(&(actual_info)->next_tetromino, TETR_SIZE);
  remove_matrix(&(actual_info)->collision_test_tetromino, TETR_SIZE);
  remove_matrix(&temp, TETR_SIZE);

  free(actual_info);
}
END_TEST

START_TEST(input) {
  ModelInfo_t *actual_info = get_info();
  actual_info->pause = 1;
  actual_info->state = Pause_state;
  userInput(Pause, true);
  ck_assert_int_eq(actual_info->user_action, Pause);
  ck_assert_int_eq(actual_info->hold, true);
  userInput(Up, false);
  ck_assert_int_eq(actual_info->user_action, Up);
  ck_assert_int_eq(actual_info->hold, false);
}
END_TEST

START_TEST(initialize) {
  ModelInfo_t *actual_info = get_info();
  userInput(Start, true);
  actual_info->user_action = Start;

  actual_info->score = -1;
  actual_info->level = -1;
  actual_info->speed = -1;
  actual_info->pause = -1;
  actual_info->high_score = -1;
  actual_info->state = Pause_state;

  create_matrix(&(actual_info)->field_base, FIELD_HEIGHT, FIELD_WIDTH);
  initialize_game(actual_info);

  ck_assert_int_eq(actual_info->score, 0);
  ck_assert_int_eq(actual_info->level, 1);
  ck_assert_int_eq(actual_info->speed, 0);
  ck_assert_int_eq(actual_info->speed, 0);
  ck_assert_int_eq(actual_info->pause, 0);
  ck_assert_int_eq(actual_info->high_score, read_score());
  ck_assert_int_eq(actual_info->state, Spawn);

  actual_info->user_action = Terminate;
  initialize_game(actual_info);
  ck_assert_int_eq(actual_info->state, Exit_state);
}
END_TEST

START_TEST(shifting) {
  ModelInfo_t *actual_info = get_info();
  actual_info->state = Moving;
  actual_info->x_position = 5, actual_info->y_position = 5;
  userInput(Left, true);

  create_matrix(&actual_info->current_tetromino, TETR_SIZE, TETR_SIZE);
  create_matrix(&actual_info->collision_test_tetromino, TETR_SIZE, TETR_SIZE);
  fill_tetromino(actual_info->current_tetromino, I_tetromino);
  move_tetromino(actual_info);

  ck_assert_int_eq(actual_info->x_position, 4);
  ck_assert_int_eq(actual_info->y_position, 5);

  actual_info->field_base[7][3] = 1, actual_info->field_base[7][9] = 1;
  userInput(Left, true);
  move_tetromino(actual_info);
  ck_assert_int_eq(actual_info->x_position, 4);

  userInput(Right, true);
  move_tetromino(actual_info);
  ck_assert_int_eq(actual_info->x_position, 5);

  userInput(Right, true);
  move_tetromino(actual_info);
  ck_assert_int_eq(actual_info->x_position, 5);

  userInput(Down, true);
  move_tetromino(actual_info);
  ck_assert_int_eq(actual_info->state, Shifting);

  actual_info->state = Moving;
  userInput(Action, true);
  fill_tetromino(actual_info->collision_test_tetromino, J_tetromino);
  move_tetromino(actual_info);
  for (int i = 0; i < TETR_SIZE; i++) {
    for (int j = 0; j < TETR_SIZE; j++) {
      ck_assert_int_eq(actual_info->current_tetromino[i][j],
                       actual_info->collision_test_tetromino[i][j]);
    }
  }
  userInput(Pause, true);
  move_tetromino(actual_info);
  ck_assert_int_eq(actual_info->state, Pause_state);
  remove_matrix(&(actual_info)->current_tetromino, TETR_SIZE);
  remove_matrix(&(actual_info)->collision_test_tetromino, TETR_SIZE);
}
END_TEST

START_TEST(clear_lines) {
  int **temp_field;
  create_matrix(&temp_field, FIELD_HEIGHT, FIELD_WIDTH);
  for (int i = FIELD_HEIGHT / 2; i < FIELD_HEIGHT; i++) {
    for (int j = 0; j < FIELD_WIDTH; j++) {
      temp_field[i][j] = 1;
    }
  }

  clear_line(temp_field, 14);
  for (int i = 0; i < (FIELD_HEIGHT / 2) + 1; i++) {
    for (int j = 0; j < FIELD_WIDTH; j++) {
      ck_assert_int_eq(temp_field[i][j], 0);
    }
  }

  clear_line(temp_field, 14);
  for (int i = 0; i < (FIELD_HEIGHT / 2) + 2; i++) {
    for (int j = 0; j < FIELD_WIDTH; j++) {
      ck_assert_int_eq(temp_field[i][j], 0);
    }
  }

  remove_matrix(&temp_field, FIELD_HEIGHT);
}
END_TEST

START_TEST(update_score_speed_level) {
  int *score = (int *)malloc(sizeof(int));
  *score = 100;
  update_score(score, 1);
  ck_assert_int_eq(*score, 200);
  update_score(score, 2);
  ck_assert_int_eq(*score, 500);
  update_score(score, 3);
  ck_assert_int_eq(*score, 1200);
  update_score(score, 4);
  ck_assert_int_eq(*score, 2700);
  free(score);

  ModelInfo_t *actual_info = get_info();
  actual_info->score = 0;
  actual_info->level = 1;
  actual_info->speed = 0;
  update_speed_and_level(actual_info);
  ck_assert_int_eq(actual_info->level, 1);
  ck_assert_int_eq(actual_info->speed, 0);

  actual_info->score = 600;
  update_speed_and_level(actual_info);
  ck_assert_int_eq(actual_info->level, 2);
  ck_assert_int_eq(actual_info->speed, 1);

  actual_info->score = 5400;
  update_speed_and_level(actual_info);
  ck_assert_int_eq(actual_info->level, 10);
  ck_assert_int_eq(actual_info->speed, 9);

  actual_info->score = 99999;
  update_speed_and_level(actual_info);
  ck_assert_int_eq(actual_info->level, 10);
  ck_assert_int_eq(actual_info->speed, 9);
}

END_TEST

START_TEST(fsm) {
  GameInfo_t test = {NULL, NULL, 0, 0, 1, 0, 0};
  if (!test.field) create_matrix(&test.field, FIELD_HEIGHT, FIELD_WIDTH);
  if (!test.next) create_matrix(&test.next, TETR_SIZE, TETR_SIZE);
  ModelInfo_t *actual_info = (ModelInfo_t *)malloc(sizeof(ModelInfo_t));
  actual_info->state = Start_state;
  actual_info->user_action = Start;
  actual_info->hold = true;
  actual_info->field_base = NULL;
  actual_info->next_tetromino = NULL;
  actual_info->next_type = O_tetromino;
  actual_info->current_tetromino = NULL;
  actual_info->collision_test_tetromino = NULL;
  actual_info->current_type = I_tetromino;
  actual_info->x_position = 5;
  actual_info->y_position = 5;
  actual_info->score = 0;
  actual_info->high_score = 0;
  actual_info->level = 1;
  actual_info->speed = 0;
  actual_info->pause = 0;
  actual_info->timer = 1;
  create_matrix(&actual_info->field_base, FIELD_HEIGHT, FIELD_WIDTH);
  create_matrix(&actual_info->next_tetromino, TETR_SIZE, TETR_SIZE);
  create_matrix(&actual_info->current_tetromino, TETR_SIZE, TETR_SIZE);
  create_matrix(&actual_info->collision_test_tetromino, TETR_SIZE, TETR_SIZE);

  run_actions_by_state(actual_info);
  actual_info->state = Start_state;

  actual_info->user_action = Start;
  actual_info->hold = true;
  run_actions_by_state(actual_info);
  ck_assert_int_eq(actual_info->state, Spawn);

  actual_info->user_action = Start;
  actual_info->hold = true;
  run_actions_by_state(actual_info);
  ck_assert_int_eq(actual_info->state, Moving);

  actual_info->state = Attaching;
  run_actions_by_state(actual_info);
  ck_assert_int_eq(actual_info->state, Spawn);

  actual_info->state = Moving;
  actual_info->hold = true;
  actual_info->x_position = 5;
  actual_info->user_action = Left;
  run_actions_by_state(actual_info);
  ck_assert_int_eq(actual_info->x_position, 4);

  actual_info->state = Moving;
  actual_info->user_action = Pause;
  run_actions_by_state(actual_info);
  ck_assert_int_eq(actual_info->state, Pause_state);
  actual_info->user_action = Pause;
  run_actions_by_state(actual_info);
  ck_assert_int_eq(actual_info->pause, 0);
  ck_assert_int_eq(actual_info->state, Moving);

  actual_info->state = Shifting;
  actual_info->x_position = 5;
  actual_info->y_position = 5;
  run_actions_by_state(actual_info);
  ck_assert_int_eq(actual_info->x_position, 5);
  ck_assert_int_eq(actual_info->y_position, 6);

  actual_info->state = Game_over;
  actual_info->score = 100;
  actual_info->high_score = 0;
  run_actions_by_state(actual_info);
  ck_assert_int_eq(actual_info->pause, 2);
  ck_assert_int_eq(actual_info->state, Start_state);

  actual_info->state = Exit_state;
  actual_info->score = 100;
  actual_info->high_score = 0;
  run_actions_by_state(actual_info);
  ck_assert_int_eq(actual_info->pause, -1);
  ck_assert_int_eq(actual_info->state, Exit_state);
  ck_assert_ptr_null(actual_info->field_base);
  ck_assert_ptr_null(actual_info->current_tetromino);
  ck_assert_ptr_null(actual_info->next_tetromino);
  ck_assert_ptr_null(actual_info->collision_test_tetromino);
  free_result(&test);
  free(actual_info);
}
END_TEST

Suite *tetris(void) {
  Suite *suite = suite_create("tetris");
  TCase *tc_core = tcase_create("tetris");

  tcase_add_test(tc_core, update_state);
  tcase_add_test(tc_core, get_model_info);
  tcase_add_test(tc_core, spawn);
  tcase_add_test(tc_core, input);
  tcase_add_test(tc_core, initialize);
  tcase_add_test(tc_core, shifting);
  tcase_add_test(tc_core, clear_lines);
  tcase_add_test(tc_core, update_score_speed_level);
  tcase_add_test(tc_core, fsm);

  suite_add_tcase(suite, tc_core);

  return suite;
}
