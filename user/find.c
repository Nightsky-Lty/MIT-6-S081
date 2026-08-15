#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"
#include "kernel/param.h"

char* cmd_argv[MAXARG];
int cmd_argc;

char* fmtname(char *path)
{
  char *p;
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;
  return p;
}

void find(char *path, char *filename,int is_exec)
{
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    if((fd = open(path, O_RDONLY)) < 0){
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }

    if(fstat(fd, &st) < 0){
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }

    switch(st.type)
    {
        case T_DEVICE:
        case T_FILE:
            if(!strcmp(fmtname(path), filename))
            {
                if(!is_exec) printf("%s\n",path);
                else
                {
                    int pid = fork();
                    if(pid == 0)
                    {
                        if(cmd_argc + 2 > MAXARG)
                        {
                            fprintf(2, "find: too many arguments\n");
                            exit(1);
                        }
                        cmd_argv[cmd_argc] = path;
                        cmd_argv[cmd_argc + 1] = 0;
                        exec(cmd_argv[0], &cmd_argv[0]);
                        exit(0);
                    }
                    wait(0);
                }
            }
            break;
        case T_DIR:
            if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf)
            {
                printf("find: path too long\n");
                break;
            }
            strcpy(buf, path);
            p = buf + strlen(buf);
            *p++ = '/';
            while(read(fd, &de, sizeof(de)) == sizeof(de))
            {
                if(de.inum == 0) continue;
                memmove(p, de.name, DIRSIZ);
                p[DIRSIZ] = 0;
                if(stat(buf, &st) < 0)
                { 
                    printf("find: cannot stat %s\n", buf);
                    continue;
                }
                if(strcmp(p, ".") == 0 || strcmp(p, "..") == 0) continue;
                find(buf, filename, is_exec);
            }
            break;
    }
    close(fd);
}
int main(int argc, char* argv[])
{
    cmd_argc = argc - 4;
    for(int i = 4; i < argc; ++i) cmd_argv[i - 4] = argv[i];
    
    if(argc < 3)
    {
        fprintf(2,"Usage: find src file\n");
        exit(1);
    }
    if(argc > 3 && !strcmp("-exec", argv[3])) find(argv[1], argv[2], 1);
    else find(argv[1], argv[2], 0);
    exit(0);
}
