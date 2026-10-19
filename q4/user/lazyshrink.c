#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(void)
{
  char *p = sbrk(8 * 4096);

  if (p == (char *)-1) {
    printf("Initial sbrk failed\n");
    exit(1);
  }

  // Allocate only selected pages.
  p[0] = 'A';
  p[4 * 4096] = 'B';

  // Shrink across both allocated and unallocated pages.
  if (sbrk(-8 * 4096) == (char *)-1) {
    printf("Shrinking failed\n");
    exit(1);
  }

  printf("Heap shrinking succeeded\n");
  exit(0);
}
