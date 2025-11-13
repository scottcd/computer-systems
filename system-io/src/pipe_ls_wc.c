// pipe_ls_wc.c
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void) {
    int p[2];
    if (pipe(p) < 0) {
        perror("pipe");
        exit(1);
    }

    pid_t pid1 = fork();
    if (pid1 < 0) {
        perror("fork");
        exit(1);
    }

    if (pid1 == 0) {
        // First child: `ls`
        // We want: ls stdout -> pipe write end
        if (dup2(p[1], STDOUT_FILENO) < 0) {
            perror("dup2 ls");
            exit(1);
        }
        close(p[0]);  // unused
        close(p[1]);  // duplicated into stdout

        char *argv[] = {"ls", NULL};
        execvp("ls", argv);
        perror("execvp ls");
        exit(1);
    }

    pid_t pid2 = fork();
    if (pid2 < 0) {
        perror("fork");
        exit(1);
    }

    if (pid2 == 0) {
        // Second child: `wc -l`
        // We want: wc stdin <- pipe read end
        if (dup2(p[0], STDIN_FILENO) < 0) {
            perror("dup2 wc");
            exit(1);
        }
        close(p[1]);  // unused
        close(p[0]);  // duplicated into stdin

        char *argv[] = {"wc", "-l", NULL};
        execvp("wc", argv);
        perror("execvp wc");
        exit(1);
    }

    // Parent: close both ends and reap children
    close(p[0]);
    close(p[1]);

    int status;
    waitpid(pid1, &status, 0);
    waitpid(pid2, &status, 0);

    return 0;
}

