#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(void)
{
    printf("Calling vmprint() to dump the current process page table:\n");

    vmprint();

    exit(0);
}
