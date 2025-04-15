#include "kernel/types.h"
#include "user/user.h"

int
main()
{
  printf("Initial memory size: %d bytes\n", memsize());

  // allocate 20 KB
  void *mem = malloc(20 * 1000); // adding at least 64KB cause of the malloc imp of xv6 (size of header is 16 usually)
  if(mem == 0) {
    printf("Memory allocation failed\n");
    exit(1,"error");
  }

  printf("After malloc: %d bytes\n", memsize());

  free(mem);

  printf("After free: %d bytes\n", memsize()); // will print same value cause this free isnt actually free but marking the memory cells as available

  exit(0,"success");
}