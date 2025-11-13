// fork_buffering.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void) {
    printf("Before fork (printf)\n");
    // fflush(stdout);  // Comment/uncomment this line during lecture

    write(STDOUT_FILENO, "Before fork (write)\n", 20);

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(1);
    }

    if (pid == 0) {
        printf("Child: hello from printf\n");
        write(STDOUT_FILENO, "Child: hello from write\n", 24);
    } else {
        printf("Parent: hello from printf\n");
        write(STDOUT_FILENO, "Parent: hello from write\n", 25);
    }

    return 0;
}

