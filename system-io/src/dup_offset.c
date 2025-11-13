// dup_offset.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(void) {
    int fd = open("dup_demo.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        perror("open");
        exit(1);
    }

    int fd2 = dup(fd);  // fd2 refers to same open file *description*
    if (fd2 < 0) {
        perror("dup");
        close(fd);
        exit(1);
    }

    dprintf(fd,  "Writing via fd\n");
    dprintf(fd2, "Writing via fd2\n");
    dprintf(fd,  "Writing via fd again\n");

    close(fd);
    close(fd2);

    return 0;
}

