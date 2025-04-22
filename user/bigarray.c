#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define ARRAY_SIZE (1 << 16) // 2^16
#define NUM_CHILDREN 4

int array[ARRAY_SIZE];

int main(int argc, char *argv[]) {
  for (int i = 0; i < ARRAY_SIZE; i++) {
    array[i] = i;
  }

  int pipes[NUM_CHILDREN][2];
  for (int i = 0; i < NUM_CHILDREN; i++) {
    if (pipe(pipes[i]) < 0) {
      fprintf(2, "ERROR: pipe creation failed\n");
      exit(1,"error");
    }
  }

  int pids[NUM_CHILDREN];
  int fork_result = forkn(NUM_CHILDREN, pids);
  if (fork_result < 0) {
    fprintf(2, "ERROR: forkn failed\n");
    exit(1,"error");
  }

  if (fork_result > 0) {
    // Child process
    int child_index = fork_result - 1;
    close(pipes[child_index][0]); // Close read end of my pipe

    int chunk = ARRAY_SIZE / NUM_CHILDREN;
    int start = chunk * child_index;
    int end = start + chunk;
    int sum = 0;
    for (int i = start; i < end; i++) {
      sum += array[i];
    }

    printf("Child #%d: sum of range [%d-%d) is %d\n", child_index + 1, start, end, sum);

    write(pipes[child_index][1], &sum, sizeof(int));
    close(pipes[child_index][1]);
    exit(0,"success"); // part 3 - passing the msg to exit
  }

  // Parent process
  int statuses[NUM_CHILDREN];
  int finished = 0;
  if (waitall(&finished, statuses) < 0) {
    fprintf(2, "ERROR: waitall failed\n");
    exit(1,"error");
  }

  if (finished != NUM_CHILDREN) {
    fprintf(2, "ERROR: incorrect number of children finished\n");
    exit(1,"error");
  }

  int total_sum = 0;
  for (int i = 0; i < NUM_CHILDREN; i++) {
    close(pipes[i][1]); // Close write end
    int part_sum = 0;
    if (read(pipes[i][0], &part_sum, sizeof(int)) != sizeof(int)) {
      fprintf(2, "ERROR: failed to read from child %d\n", i);
      exit(1,"error");
    }
    close(pipes[i][0]);
    total_sum += part_sum;
  }

  printf("Parent: total sum is %d\n", total_sum);
  exit(0,"success"); // part 3 - passing the msg to exit
}
