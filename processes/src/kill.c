#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

int main() {
    pid_t pid;

    /* Child sleeps until SIGKILL signal received, then dies */
    if ((pid = fork()) == 0) {
        pause();                                        /* Wait for a signal to arrive */
        printf("Control should never reach here!");
        exit(0);
    }

    /* Parent sends a SIGKILL signal to a child */
    kill(pid, SIGKILL);
    exit(0);
}
