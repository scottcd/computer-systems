#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

void sigint_handler(int sig) {
    printf("Caught SIGINT!\n");
    exit(0);
}

int main() {
    /* Install the SIGINT handler */
    if (signal(SIGINT, sigint_handler) == SIG_ERR) {
        printf("Signal Error\n");
        exit(2);
    }

    pause(); /* Wait for the receipt of a signal */

    return 0;
}
