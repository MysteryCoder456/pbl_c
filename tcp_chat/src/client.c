#include "common.h"
#include <stdbool.h>

#define SERVER_ADDR "127.0.0.1"
#define SERVER_PORT "8000"

volatile bool keepRunning = false;
int sockfd;

void interruptHandler() {
    // Close server listener socket
    close(sockfd);
    keepRunning = false;
}

int main() {
    // Generate server address info
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(SERVER_ADDR, SERVER_PORT, &hints, &res) != 0) {
        printf("Failed to get server address info\n");
        return 1;
    }

    // Create socket and connect to server
    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (connect(sockfd, res->ai_addr, res->ai_addrlen) != 0) {
        printf("Failed to connect to server\n");
        return 1;
    }
    keepRunning = true;
    printf("Connected to server!\n");
    freeaddrinfo(res);

    signal(SIGINT, interruptHandler);

    while (keepRunning) {
        // Send a goofy ahh message to server
        msg_send(sockfd, 0, "wassup meow", 11);

        // Receive server response
        char *buf;
        message_size n;
        if (msg_recv(sockfd, (void **)&buf, &n) == -1)
            break;

        printf("Server said: ");
        printstr(buf, n);
    }

    printf("Disconnected from server, exiting...\n");
    close(sockfd);
    return 0;
}
