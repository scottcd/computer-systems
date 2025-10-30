#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <errno.h>

#define N 20

void unix_error(char *msg) {
    fprintf(stderr, "fork error: %s: %d\n", msg, strerror(errno));
    exit(0);
}

int main() {
    int status, i;
    pid_t pid[N], retpid;
    
    /* Parent creates N children */
    for (i = 0; i < N; i++) {
        if ((pid[i] = fork()) == 0) /* Child */
            exit(100 + i);
    }

    /* At this point, we have N zombie processes. */

    /* Parent reaps N children in no particular order */
    i = 0;
    while ((retpid = waitpid(pid[i++], &status, 0)) > 0) {
        if (WIFEXITED(status)) {
            printf("child %d terminated normally with exit status=%d\n",
                retpid, WEXITSTATUS(status));
        }
        else {
            printf("child %d terminated abnormally\n", retpid);
        }
    }

    /* The only normal termination is if there are no more children */
    if(errno != ECHILD) {
        unix_error("waitpid error");
    }

    exit(0);
}
