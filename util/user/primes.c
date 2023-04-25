#include "kernel/types.h"
#include "user/user.h"

int leftfd[2];
int rightfd[2];

int create_next_sieve(void)
{   
    int pid;
    int init_p;
    int p;

    leftfd[0] = rightfd[0];
    leftfd[1] = rightfd[1];
    close(leftfd[1]);
    
    if (read(leftfd[0], &init_p, sizeof(int)) == 0)
        return 0;

    printf("prime %d\n", init_p);

    if (pipe(rightfd) < 0) {
        fprintf(2, "primes: Can't create pipe\n");
        return -1;
    }

    pid = fork();

    if (pid < 0) {
        fprintf(2, "primes: Failed to fork process\n");
        return -1;
    }

    if (pid > 0) {
        close(rightfd[0]);

        while (read(leftfd[0], &p, sizeof(int)) != 0) {
            if (p % init_p == 0)
                continue;

            write(rightfd[1], &p, sizeof(int));
        }
        close(rightfd[1]);
    } else {
        return create_next_sieve();
    }

    return 0;
}

int main(void)
{   
    int pid;
    
    if (pipe(rightfd) < 0) {
        fprintf(2, "primes: Can't create pipe\n");
        return -1;
    }

    pid = fork();
    if (pid < 0) {
        fprintf(2, "primes: Failed to fork process\n");
        return -1;
    }

    if (pid > 0) {              /* parent */
        close(rightfd[0]);

        printf("prime 2\n");

        for (int p = 3; p <= 35; p++) {
            if ((p & 1) == 0) 
                continue;
            
            write(rightfd[1], &p, sizeof(int));
        }
        close(rightfd[1]);
    } else {                    /* child */
        if (create_next_sieve() < 0)
            exit(1);
    }
    wait(0);

    exit(0);
}