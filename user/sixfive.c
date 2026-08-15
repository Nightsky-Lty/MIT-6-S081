#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"

char str[16] = " -\r\t\n./,";

void sixfive(char* file)
{
    int num = 0, n, fd = open(file, O_RDONLY),valid = 1, one = 0;
    char c;
    if(fd < 0)
    {
        fprintf(2, "error filename");
        exit(1);
    }
    while((n = read(fd, &c, 1)) == 1)
    {
        if(strchr(str, c))
        {
            if(valid && one && (num % 5 == 0 || num % 6 == 0)) fprintf(1, "%d\n",num);
            num = 0;
            valid = 1;
            one = 0;
        }
        else if(c >= '0' && c <= '9')
        {
            num = num * 10 + c - '0';
            one = 1;
        }
        else
        {
            num = 0;
            valid = 0;
        }
    }
    if(valid && one && (num % 5 == 0 || num % 6 == 0)) fprintf(1, "%d\n",num);
    if(n < 0)
    {
        fprintf(2, "sixfive: read error\n");
        exit(1);
    }
    close(fd);
}

int main(int argc, char* argv[])
{
    if(argc < 2)
    {
        fprintf(2, "Usage: sixfive files...\n");
        exit(1);
    }
    for(int i = 1; i < argc; ++i)
        sixfive(argv[i]);
    exit(0);
}