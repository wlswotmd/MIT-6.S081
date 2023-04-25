#include "kernel/types.h"
#include "kernel/param.h"
#include "user/user.h"

#define BUFSIZ 256
#define NULL (void *)0

int strcspn(char *str, char *delim)
{
    int i;
    for (i = 0; str[i] != 0; i++) {
        for (int j = 0; delim[j] != 0; j++) {
            if (str[i] == delim[j])
                return i;
        }
    }

    return i;
}

char *strtok(char *str, char *delim)
{
    static char *olds;
    char *end;
    
    if (str == NULL)
        str = olds;

    if (*str == 0) {
        olds = str;
        return NULL;
    }

    end = str + strcspn(str, delim);
    *end = 0;

    olds = ++end;

    return str;
}

int readall(int fd, void *buf, int size)
{
    int todo;
    int nread;

    todo = size;
    while (todo > 0) {
        nread = read(fd, buf + size - todo, todo);
        if (nread <= 0)
            break;
        todo -= nread;
    }

    return size - todo;
}

void xargs(char *exe, int eargc, char *eargv[])
{
    char *argv[MAXARG];
    char buf[BUFSIZ];
    char *arg;
    int pid;
    int nread;

    memset(argv, 0, MAXARG * sizeof(char *));
    memcpy(argv, eargv, eargc * sizeof(char *));

    nread = readall(0, buf, BUFSIZ);
    // printf("%s\n", buf);
    arg = strtok(buf, "\x09\x0a\x0b\x0c\x0d\x20");

    if (nread > 0)
        argv[eargc++] = arg;

    while (eargc < MAXARG) {
        arg = strtok(NULL, "\x09\x0a\x0b\x0c\x0d\x20");
        if (arg == NULL)
            break;
        argv[eargc++] = arg;
    }

    pid = fork();
    if (pid < 0) {
        fprintf(2, "xargs: Failed to fork process\n");
        return;
    }
        
    if (pid > 0)    /* parent */
        wait(0);
    else            /* child */
    {
        // for (int i = 0; i < MAXARG; i++)
        //     printf("%s\n", argv[i]);
        exec(exe, argv);  
    }
                  
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(2, "Usage: xargs cmd args...\n");
        exit(1);
    }

    if (argc > MAXARG + 2) {
        fprintf(2, "xargs: Too many args\n");
        exit(1);
    }

    xargs(argv[1], argc - 1, &argv[1]);

    exit(0);
}