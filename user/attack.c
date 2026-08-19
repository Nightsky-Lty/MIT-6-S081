#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"
#include "kernel/riscv.h"

int
main(int argc, char *argv[])
{
    char *p = sbrk(8 * 4096);
    if(p == SBRK_ERROR)
        exit(1);
    for(int i = 0; i < 8 * 4096 - 16; ++i, ++p)
    {
        if(strcmp("This may help.", p) == 0)
        {
            printf("%s\n", p + 16);
            exit(0);
        }
    }
    exit(1);
}
