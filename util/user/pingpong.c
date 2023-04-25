#include "kernel/types.h"
#include "user/user.h"

int main(void)
{
    int pipefd[2];
    char rcv;
    int pid;

    if (pipe(pipefd) < 0) {
        fprintf(2, "pingpong: Can't create pipe\n");
        exit(1);
    }

    pid = fork();
    if (pid < 0) {
        fprintf(2, "pingpong: Failed to fork process\n");
        exit(1);
    }
    
    if (pid > 0) {  /* parent */
        write(pipefd[1], "P", 1);

        if (read(pipefd[0], &rcv, 1) != 1) {
            fprintf(2, "pingpong: Failed to read from pipe\n");
            exit(1);
        }

        printf("%d: received pong\n", getpid());
    } else {        /* child */
        if (read(pipefd[0], &rcv, 1) != 1) {
            fprintf(2, "pingpong: Failed to read from pipe\n");
            exit(1);
        }

        printf("%d: received ping\n", getpid());

        write(pipefd[1], &rcv, 1);
    } 

    exit(0);
}