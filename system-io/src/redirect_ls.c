// redirect_ls.c
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(void) {
    printf("Redirecting `ls` output to out.txt\n");
    fflush(stdout);

    int fd = open("out.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        perror("open");
        exit(1);
    }

    // Make fd become the new stdout (fd 1)
    if (dup2(fd, STDOUT_FILENO) < 0) {
        perror("dup2");
        close(fd);
        exit(1);
    }
    close(fd);  // fd not needed anymore; stdout now goes to out.txt

    // From this point on, anything written to stdout goes to out.txt
    printf("This line goes into out.txt, not the terminal\n");
    fflush(stdout);

    char *argv[] = {"ls", "-l", NULL};
    execvp("ls", argv);

    // Only reached if exec fails
    perror("execvp");
    exit(1);
}

