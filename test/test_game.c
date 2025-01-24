#include "test.h"

int main() {
  int failed = 0;
  int success = 0;

  SRunner *sr = srunner_create(tetris());
  srunner_set_fork_status(sr, CK_NOFORK);
  srunner_run_all(sr, CK_NORMAL);
  failed += srunner_ntests_failed(sr);
  success += srunner_ntests_run(sr);
  srunner_free(sr);

  printf("\033[31m========= FAILED : %-3d =========\033[0m\n", failed);
  printf("\033[32m========= SUCCESS : %-3d =========\033[0m\n", success);
  return (failed == 0) ? FAILURE : SUCCESS;
}