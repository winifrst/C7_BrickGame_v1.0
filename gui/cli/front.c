/**
 * @file front.c
 * @brief Frontend functions for Tetris, handling the game loop, UI rendering,
 * and user input.
 *
 * This file defines functions responsible for running the game loop, displaying
 * the game field, handling user input, and managing game UI.
 */
#include "front.h"

/**
 * @brief The main function to initialize the game and start the game loop.
 *
 * Initializes the ncurses screen, sets the terminal mode, and starts the game
 * loop by calling `run_game_loop`. After the loop ends, the ncurses session is
 * terminated.
 *
 * @return An integer exit status (0 for success).
 */
int main() {
  initscr();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);
  curs_set(0);
  nodelay(stdscr, TRUE);

  run_game_loop();

  endwin();
  return 0;
}

/**
 * @brief Runs the main game loop.
 *
 * This function manages the game loop, where it continually updates the game
 * state, renders the game field, and handles user input. It keeps the game
 * running until it is paused or terminated. The game info is updated and
 * displayed on the screen through various windows (`game_win`, `next_win`,
 * `info_win`).
 *
 * @return A boolean indicating if the game loop is running (`true`) or not
 * (`false`).
 */
bool run_game_loop() {
  UserAction_t user_action;
  bool is_ok = true;
  Interface_t windows;
  windows.game_win = newwin(FIELD_WIDTH * 2 + 2, FIELD_HEIGHT + 2, 1, 1);
  windows.next_win = newwin(7, 18, 1, FIELD_WIDTH * 2 + 3);
  windows.info_win = newwin(15, 18, 8, FIELD_WIDTH * 2 + 3);

  start_color();
  init_pair((short)1, COLOR_BLACK, COLOR_YELLOW);
  init_pair((short)2, COLOR_BLACK, COLOR_CYAN);
  init_pair((short)3, COLOR_BLACK, COLOR_MAGENTA);
  init_pair((short)4, COLOR_BLACK, COLOR_GREEN);
  init_pair((short)5, COLOR_BLACK, COLOR_RED);
  init_pair((short)6, COLOR_BLACK, COLOR_BLUE);
  init_pair((short)7, COLOR_BLACK, COLOR_WHITE);

  while (is_ok) {
    GameInfo_t gameInfo = updateCurrentState();

    if (gameInfo.pause != EXIT_GAME) {
      print_field(&gameInfo, &windows);
      print_next(&gameInfo, &windows);
      print_info(&gameInfo, &windows);

      int key = getch();
      bool hold = (key != ERR);
      user_action = get_action(key);
      userInput(user_action, hold);

      free_game_info(&gameInfo);

      refresh();
      napms(5);
    } else
      is_ok = false;
  }

  delwin(windows.game_win);
  delwin(windows.next_win);
  delwin(windows.info_win);

  return 0;
}

/**
 * @brief Frees dynamically allocated memory for the `gameInfo` structure.
 *
 * This function deallocates the memory for the game field and next tetromino
 * arrays in the `gameInfo` structure.
 *
 * @param gameInfo A pointer to the `GameInfo_t` structure containing the game
 * data to free.
 */
void free_game_info(GameInfo_t *gameInfo) {
  if (gameInfo->field) {
    for (int i = 0; i < FIELD_HEIGHT; i++) {
      free(gameInfo->field[i]);
      gameInfo->field[i] = NULL;
    }
    free(gameInfo->field);
    gameInfo->field = NULL;

    if (gameInfo->next) {
      for (int i = 0; i < TETR_SIZE; i++) {
        free(gameInfo->next[i]);
        gameInfo->next[i] = NULL;
      }
      free(gameInfo->next);
      gameInfo->next = NULL;
    }
  }
}

/**
 * @brief Prints the game field on the screen.
 *
 * This function displays the current game field in the `game_win` window. It
 * handles rendering of the tetrominoes and empty spaces, adjusting the display
 * based on whether the game is paused or not.
 *
 * @param gameInfo A pointer to the `GameInfo_t` structure containing the game
 * field data.
 * @param windows A pointer to the `Interface_t` structure containing the
 * ncurses window pointers.
 */
void print_field(const GameInfo_t *gameInfo, Interface_t *windows) {
  werase(windows->game_win);
  box(windows->game_win, 0, 0);
  for (int y = 0; y < FIELD_HEIGHT; ++y) {
    for (int x = 0; x < FIELD_WIDTH; ++x) {
      if (gameInfo->field[y][x]) {
        if (gameInfo->pause) {
          mvwaddch(windows->game_win, y + 1, x * 2 + 1, '[');
          mvwaddch(windows->game_win, y + 1, x * 2 + 2, ']');
        } else {
          wattron(windows->game_win, COLOR_PAIR(gameInfo->field[y][x]));
          mvwaddch(windows->game_win, y + 1, x * 2 + 1, ' ');
          mvwaddch(windows->game_win, y + 1, x * 2 + 2, ' ');
          wattroff(windows->game_win, COLOR_PAIR(gameInfo->field[y][x]));
        }
      } else {
        mvwaddch(windows->game_win, y + 1, x * 2 + 1, ' ');
        mvwaddch(windows->game_win, y + 1, x * 2 + 2, ' ');
      }
    }
  }
  wrefresh(windows->game_win);
}

/**
 * @brief Prints the next tetromino in the upcoming window.
 *
 * This function displays the next tetromino that will fall in the `next_win`
 * window. It visualizes the upcoming tetromino and adjusts the display based on
 * whether the game is paused.
 *
 * @param gameInfo A pointer to the `GameInfo_t` structure containing the next
 * tetromino data.
 * @param windows A pointer to the `Interface_t` structure containing the
 * ncurses window pointers.
 */
void print_next(const GameInfo_t *gameInfo, Interface_t *windows) {
  werase(windows->next_win);
  box(windows->next_win, 0, 0);
  for (int y = 0; y < TETR_SIZE; ++y) {
    for (int x = 0; x < TETR_SIZE; ++x) {
      if (gameInfo->next[y][x]) {
        if (gameInfo->pause) {
          mvwaddch(windows->next_win, y + 1, x * 2 + 4, '[');
          mvwaddch(windows->next_win, y + 1, x * 2 + 5, ']');
        } else {
          wattron(windows->next_win, COLOR_PAIR(gameInfo->next[y][x]));
          mvwaddch(windows->next_win, y + 1, x * 2 + 4, ' ');
          mvwaddch(windows->next_win, y + 1, x * 2 + 5, ' ');
          wattroff(windows->next_win, COLOR_PAIR(gameInfo->next[y][x]));
        }
      } else {
        mvwaddch(windows->next_win, y + 1, x * 2 + 4, ' ');
        mvwaddch(windows->next_win, y + 1, x * 2 + 5, ' ');
      }
    }
  }

  wrefresh(windows->next_win);
}

/**
 * @brief Prints game information (score, level, speed) on the screen.
 *
 * This function updates and displays important game information, such as the
 * current score, high score, game level, and speed, in the `info_win` window.
 * The display changes depending on whether the game is paused.
 *
 * @param gameInfo A pointer to the `GameInfo_t` structure containing the game
 * data.
 * @param windows A pointer to the `Interface_t` structure containing the
 * ncurses window pointers.
 */
void print_info(const GameInfo_t *gameInfo, Interface_t *windows) {
  int offset_high = offset_counter(gameInfo->high_score);
  int offset = offset_counter(gameInfo->score);
  werase(windows->info_win);
  box(windows->info_win, 0, 0);
  if (gameInfo->pause == 1) {
    mvwprintw(windows->info_win, 1, 4, "= PAUSE =");
    mvwprintw(windows->info_win, 3, 4, "press  'P'");
    mvwprintw(windows->info_win, 4, 4, "to  resume");
  } else if (gameInfo->pause == 2) {
    mvwprintw(windows->info_win, 1, 4, "= TETRIS =");
    mvwprintw(windows->info_win, 3, 3, "press 'ENTER'");
    mvwprintw(windows->info_win, 4, 5, "to start");
  }
  if (gameInfo->pause) {
    mvwprintw(windows->info_win, 5, 4, "press 'ESC'");
    mvwprintw(windows->info_win, 6, 6, "to exit");
    mvwprintw(windows->info_win, 9, 5, "CONTROL");
    mvwprintw(windows->info_win, 10, 2, "< arrow keys >");
    mvwprintw(windows->info_win, 11, 8, "v");
    mvwprintw(windows->info_win, 12, 5, "'space'");
    mvwprintw(windows->info_win, 13, 4, "to rotate");
  } else {
    mvwprintw(windows->info_win, 2, 4, "high score");
    mvwprintw(windows->info_win, 3, 8 - offset_high, "%d",
              gameInfo->high_score);
    mvwprintw(windows->info_win, 5, 6, "score");
    mvwprintw(windows->info_win, 6, 8 - offset, "%d", gameInfo->score);
    mvwprintw(windows->info_win, 8, 6, "speed");
    mvwprintw(windows->info_win, 9, 6, "%3.1f x",
              (float)gameInfo->speed * 0.3 + 1);
    mvwprintw(windows->info_win, 11, 6, "level");
    mvwprintw(windows->info_win, 12, 4, "%5d", gameInfo->level);
  }
  wrefresh(windows->info_win);
}

/**
 * @brief Maps user input keys to corresponding actions.
 *
 * This function converts a key pressed by the user into a corresponding action
 * that is processed by the game. These actions include movement (up, down,
 * left, right), starting, pausing, etc.
 *
 * @param key The key pressed by the user.
 *
 * @return The corresponding user action based on the key pressed.
 */
UserAction_t get_action(int key) {
  UserAction_t action;
  switch (key) {
    case ENTER_KEY:
      action = Start;
      break;
    case 'p':
    case 'P':
      action = Pause;
      break;
    case 'q':
    case 'Q':
      action = Terminate;
      break;
    case KEY_LEFT:
      action = Left;
      break;
    case KEY_RIGHT:
      action = Right;
      break;
    case KEY_DOWN:
      action = Down;
      break;
    case SPACE_KEY:
      action = Action;
      break;
    case KEY_UP:
      action = Up;
      break;
    default:
      break;
  }
  return action;
}

/**
 * @brief Converts a number to its digit length for formatting purposes.
 *
 * This function counts the number of digits in an integer number and returns
 * half of the count, which is used for formatting purposes in the score
 * display.
 *
 * @param number The integer number whose digit length is to be calculated.
 *
 * @return The number of digits divided by 2.
 */
int offset_counter(int number) {
  int count = 0;

  while (number > 0) {
    count++;
    number /= 10;
  }

  return count / 2;
}
