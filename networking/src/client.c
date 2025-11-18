// client.c
// Simple TCP client: connects to server, sends one line, prints reply.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define SERVER_PORT 8080
#define BUF_SIZE    1024

static void die(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
    int sockfd;
    struct sockaddr_in server_addr;
    char buf[BUF_SIZE];

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <server-ip>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *server_ip = argv[1];

    // 1. Create TCP socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
        die("socket");

    // 2. Fill in server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port   = htons(SERVER_PORT);
    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
        fprintf(stderr, "Invalid address: %s\n", server_ip);
        exit(EXIT_FAILURE);
    }

    // 3. Connect
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
        die("connect");

    printf("Connected to %s:%d\n", server_ip, SERVER_PORT);
    printf("Enter a line: ");
    if (fgets(buf, sizeof(buf), stdin) == NULL) {
        fprintf(stderr, "No input.\n");
        close(sockfd);
        return 0;
    }

    // 4. Send to server
    size_t len = strlen(buf);
    if (send(sockfd, buf, len, 0) == -1)
        die("send");

    // 5. Receive echo
    ssize_t nread = recv(sockfd, buf, sizeof(buf) - 1, 0);
    if (nread == -1)
        die("recv");
    buf[nread] = '\0';

    printf("Server replied: %s", buf);

    close(sockfd);
    return 0;
}

