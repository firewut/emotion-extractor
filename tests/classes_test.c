#include <stdio.h>
#include "minunit.h"

#include <classes.h>

int tests_run = 0;

unsigned int columns = 5;
unsigned int rows = 5;


static char * test_grid_new_and_destroy() {
  struct Grid* grid = new_grid(columns, rows);

  mu_assert(
    "grid is not empty",
    grid != NULL
  );
  destroy_grid(grid);

  return 0;
}

static char * test_grid_nodes_count() {
  struct Grid* grid = new_grid(columns, rows);

  int grid_nodes_count = rows * columns;
  mu_assert(
    "grid nodes count mismatch",
    grid->nodes_count == grid_nodes_count
  );
  destroy_grid(grid);

  return 0;
}

static char * test_grid_node_hash(){
  struct Grid* grid = new_grid(columns, rows);

  char *node_hash = get_node_hash(
    get_grid_node(grid, 1, 2)
  );

  mu_assert(
    "grid node hash mismatch",
    strcmp(node_hash, "1-2") == 0
  );
  destroy_grid(grid);

  return 0;
}

static char * all_tests() {
  mu_run_test(test_grid_new_and_destroy);
  mu_run_test(test_grid_nodes_count);
  mu_run_test(test_grid_node_hash);

  return 0;
}

int main(int argc, char **argv) {
  char *result = all_tests();
  if (result != 0) {
    printf("%s\n", result);
  } else {
    printf("ALL TESTS PASSED\n");
  }
  printf("Tests run: %d\n", tests_run);

  return result != 0;
}
