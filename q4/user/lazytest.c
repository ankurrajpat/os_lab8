#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(void)
{
  char *p;
  int size = 32 * 1024 * 1024;
  int fd;

  printf("Lazy allocation test starting\n");

  p = sbrk(size);
  if (p == (char *)-1) {
    printf("sbrk failed\n");
    exit(1);
  }

  printf("Reserved 32 MB of virtual memory\n");

  // Touch only four pages.
  p[0] = 'A';
  p[4 * 1024 * 1024] = 'B';
  p[16 * 1024 * 1024] = 'C';
  p[31 * 1024 * 1024] = 'D';

  printf("Touched four pages successfully\n");

  // Test fork with lazily allocated memory.
  int pid = fork();
  if (pid < 0) {
    printf("fork failed\n");
    exit(1);
  }

  if (pid == 0) {
    printf("Child process created successfully\n");
    exit(0);
  }

  wait(0);

  // Test a system call writing into an untouched page.
  fd = open("README", 0);
  if (fd < 0) {
    printf("Could not open README\n");
    exit(1);
  }

  int n = read(fd, p + 8 * 1024 * 1024, 32);
  close(fd);

  if (n < 0) {
    printf("Read into untouched page failed\n");
    exit(1);
  }

  printf("Read into untouched page succeeded: %d bytes\n", n);
  printf("Lazy allocation test completed\n");

  exit(0);
}
