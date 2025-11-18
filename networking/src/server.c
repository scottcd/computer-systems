// server.c
// Simple TCP echo server: accepts one client at a time and echoes lines back.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define LISTEN_PORT 8080
#define BACKLOG     5
#define BUF_SIZE    1024

static void die(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

int main(void) {
    int listen_fd, conn_fd;
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    socklen_t client_len;
    char buf[BUF_SIZE];

    // 1. Create a TCP socket (IPv4)
    listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd == -1)
        die("socket");

    // 2. (OPTIONAL) Allow quick reuse of the address after the program exits
    int optval = 1;
    if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1)
        die("setsockopt");

    // 3. Bind to 0.0.0.0:LISTEN_PORT
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family      = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY); // listen on all interfaces
    server_addr.sin_port        = htons(LISTEN_PORT);

    if (bind(listen_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
        die("bind");

    // 4. Mark socket as listening
    if (listen(listen_fd, BACKLOG) == -1)
        die("listen");

    printf("Server listening on port %d...\n", LISTEN_PORT);

    // 5. Main accept loop
    while (1) {
        client_len = sizeof(client_addr);
        conn_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &client_len);
        if (conn_fd == -1) {
            perror("accept");
            continue;
        }

        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));
        printf("Accepted connection from %s:%d\n",
               client_ip, ntohs(client_addr.sin_port));

        // 6. Echo loop for this client
        while (1) {
            ssize_t nread = recv(conn_fd, buf, sizeof(buf) - 1, 0);
            if (nread == -1) {
                perror("recv");
                break;
            } else if (nread == 0) {
                // client closed the connection
                printf("Client disconnected.\n");
                break;
            }

            buf[nread] = '\0'; // make it a C string
            printf("Received: %s", buf);

            // Echo back to client
            ssize_t nwritten = send(conn_fd, buf, nread, 0);
            if (nwritten == -1) {
                perror("send");
                break;
            }
        }

        close(conn_fd);
    }

    // Not reached in this simple example
    close(listen_fd);
    return 0;
}

