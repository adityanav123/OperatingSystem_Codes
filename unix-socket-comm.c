#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>

#define SOCK_PATH "/tmp/temp-socket"
#define MAX_BUFF_SZ 1024

void do_server() {
    int server_fd, client_fd;
    struct sockaddr_un addr; // Unix Socket
    char buffer[MAX_BUFF_SZ];

    // Socket
    server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("socket creation failed!");
        exit(1);
    }

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCK_PATH, sizeof(addr.sun_path) - 1);
    unlink(SOCK_PATH); // removing existing socket file

    // Bind
    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        perror("binding failed!");
        exit(1);
    }

    // Listening
    if (listen(server_fd, 5) == -1) {
        perror("listening failed!");
        exit(1);
    }

    printf("Server : waiting for connection...\n");
    client_fd = accept(server_fd, NULL, NULL);
    if (client_fd == -1) {
        perror("accept failed!");
        exit(1);
    }

    // reading message
    int n = read(client_fd, buffer, MAX_BUFF_SZ);
    if (n > 0) {
        buffer[n] = '\0';
        printf("Server<Recv>: '%s'\n", buffer);

        char *resp = "Hello From Server!";
        write(client_fd, resp, strlen(resp));
    }

    // Cleaning Up
    close(client_fd);
    close(server_fd);
    unlink(SOCK_PATH);
}

void do_client() {
    int sock_fd;
    struct sockaddr_un addr;
    char buffer[MAX_BUFF_SZ];

    sock_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock_fd == -1) {
        perror("socket failed!");
        exit(1);
    }

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCK_PATH, sizeof(addr.sun_path) - 1);

    if (connect(sock_fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        perror("connection to server failed!");
        exit(1);
    }

    char *message = "Client Connected!";
    write(sock_fd, message, strlen(message));

    int n = read(sock_fd, buffer, MAX_BUFF_SZ);
    if (n > 0) {
        buffer[n] = '\0';
        printf("Client<Recv>: '%s'\n", buffer);
    }

    close(sock_fd);
}

int main(int argc, char *argv[]) {
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork failed!");
        exit(1);
    }

    if (pid == 0) {
        sleep(1);
        do_client();
    } else {
        do_server();
        wait(NULL); // wait for child to respond
    }
    return 0;
}
