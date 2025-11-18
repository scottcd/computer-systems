# TCP Client-Server Guide

## Structs
- `struct sockaddr_in`: IPv4-specific socket address used by both client and server.
  - `/usr/include/netinet/in.h`
- `struct in_addr`: Holds a 32-bit IPv4 address (in network byte order).
  - `/usr/include/netinet/in.h`
- `struct sockaddr`: Generic socket address type required by POSIX functions (bind, connect, accept).
  - `/usr/include/sys/socket.h`
- Supporting Types
  - `sa_family_t` : `/usr/include/sys/socket.h` or `/usr/include/linux/socket.h`
  - `in_port_t` & `in_addr_t`: `/usr/include/sys/socket.h` or `/usr/include/linux/socket.h`

## Functions
- `socket()`: Creates a socket file descriptor.
  - `/usr/include/sys/socket.h` or `/usr/include/linux/socket.h`
- `bind()`: Associates the socket with an IP/port.
  - `/usr/include/sys/socket.h` or `/usr/include/linux/socket.h`
- `listen()`: Marks the socket as passive (ready to accept connections).
  - `/usr/include/sys/socket.h` or `/usr/include/linux/socket.h`
- `accept()`: Returns a new connected socket for a client.
  - `/usr/include/sys/socket.h` or `/usr/include/linux/socket.h`
- `connect()`: Initiates an active connection to a server.
  - `/usr/include/sys/socket.h` or `/usr/include/linux/socket.h`
- `send()`/`recv()`: Sends and receives bytes over a TCP connection.
  - `/usr/include/sys/socket.h` or `/usr/include/linux/socket.h`
- `inet_pton()`/`inet_ntop()`: Converts between human-readable and binary IPv4 addresses.
  - `/usr/include/arpa/inet.h`

## Relevant man pages
- `man 2 socket`
- `man 2 bind`
- `man 2 listen`
- `man 2 accept`
- `man 2 connect`
- `man 3 inet_pton`
