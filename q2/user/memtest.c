#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void
print_meminfo(char *message)
{
    struct meminfo mi;

    if (getmeminfo(&mi) < 0) {
        printf("getmeminfo failed\n");
        return;
    }

    printf("\n%s\n", message);
    printf("Total physical pages: %d\n", (int)mi.total_pages);
    printf("Free physical pages: %d\n", (int)mi.free_pages);
    printf("Process pages: %d\n", (int)mi.process_pages);
    printf("Page-table pages: %d\n", (int)mi.page_table_pages);
}

int
main(void)
{
    int pid;
    char *memory;

    // 1. Display initial memory statistics.
    print_meminfo("=== Initial Memory Statistics ===");

    // 2. Allocate memory in the parent process.
    printf("\nAllocating 10 pages in the parent process...\n");

    memory = sbrk(10 * 4096);

    if (memory == (char *)-1) {
        printf("sbrk failed\n");
        exit(1);
    }

    // Touch the allocated memory so the pages are actually allocated.
    for (int i = 0; i < 10 * 4096; i += 4096) {
        memory[i] = 1;
    }

    print_meminfo("=== After Parent Allocation ===");

    // 3. Create a child process.
    printf("\nCreating a child process...\n");

    pid = fork();

    if (pid < 0) {
        printf("fork failed\n");
        exit(1);
    }

    if (pid == 0) {
        // Child process.
        print_meminfo("=== Inside Child Process ===");

        printf("Child exiting...\n");
        exit(0);
    }

    // 4. Wait for the child to finish so its memory can be reclaimed.
    wait(0);

    print_meminfo("=== After Child Exit and Wait ===");

    // 5. Release the additional memory allocated by the parent.
    printf("\nReleasing the 10 allocated pages...\n");

    if (sbrk(-10 * 4096) == (char *)-1) {
        printf("Memory release failed\n");
        exit(1);
    }

    print_meminfo("=== Final Memory Statistics ===");

    exit(0);
}

