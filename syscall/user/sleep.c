#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
    int sec;

    if (argc != 2) {
        fprintf(2, "Usage: sleep seconds\n");
        exit(1);
    }

    sec = atoi(argv[1]);
    if (sleep(sec) < 0) {
        fprintf(2, "sleep: Can't sleep for %d seconds\n", sec);
        exit(1);
    } 

    exit(0);
}