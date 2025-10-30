#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int main() {
    pid_t pid;
    int x = 1;

    pid = fork();

    if (pid == 0) {
    /* Child */
        printf ("Child: %d\n", ++x);
        exit(0);
    }

    /* Parent */
    printf ("Parent: %d\n", --x);
    exit(0);
}
