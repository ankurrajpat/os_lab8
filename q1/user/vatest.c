#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int global_var = 100;

int
main(void)
{
  int local_var = 200;
  int *heap_var;

  printf("\n=== VA to PA Translation Test ===\n");

  // Test 1: Global variable
  printf("\n--- Test 1: Global Variable ---\n");
  printf("VA: %lx\n", (uint64)&global_var);
  printf("PA: %lx\n", va2pa((uint64)&global_var));

  // Test 2: Local variable
  printf("\n--- Test 2: Local Variable ---\n");
  printf("VA: %lx\n", (uint64)&local_var);
  printf("PA: %lx\n", va2pa((uint64)&local_var));

  // Test 3: Heap variable
  printf("\n--- Test 3: Heap Variable ---\n");
  heap_var = malloc(sizeof(int));

  if (heap_var == 0) {
    printf("Memory allocation failed\n");
    exit(1);
  }

  *heap_var = 300;
  printf("VA: %lx\n", (uint64)heap_var);
  printf("PA: %lx\n", va2pa((uint64)heap_var));

  free(heap_var);

  // Test 4: Invalid/unmapped virtual address
  printf("\n--- Test 4: Invalid Address ---\n");
  printf("Testing an unmapped virtual address...\n");
  va2pa(0x7fffffff);

  // Test 5: Parent and child address translation
  printf("\n--- Test 5: Fork Test ---\n");

  printf("Parent global VA: %lx\n", (uint64)&global_var);
  printf("Parent global PA: %lx\n",
         va2pa((uint64)&global_var));

  int pid = fork();

  if (pid < 0) {
    printf("Fork failed\n");
    exit(1);
  }

  if (pid == 0) {
    printf("Child global VA: %lx\n", (uint64)&global_var);
    printf("Child global PA: %lx\n",
           va2pa((uint64)&global_var));
    exit(0);
  } else {
    wait(0);
  }

  printf("\n=== All Tests Completed ===\n");
  exit(0);
}
