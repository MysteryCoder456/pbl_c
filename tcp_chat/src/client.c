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

    // Accept username
    printf("Enter a username: ");
    char username[MAX_USERNAME + 2];
    fgets(username, MAX_USERNAME + 2, stdin);

    // Removes trailing linebreak
    size_t usernameLength = strlen(username) - 1;
    username[usernameLength] = '\0';
    printf("Your username is: %s\n", username);

    // Register username with server
    msg_send(sockfd, MSG_REGISTER, username, usernameLength);

    while (keepRunning) {
        // Send a goofy ahh message to server
        msg_send(sockfd, MSG_CHAT, "wassup meow", 11);

        // Receive server response
        void *buf;
        message_size n;

        // Process message
        switch (msg_recv(sockfd, (void **)&buf, &n)) {
        case MSG_CHAT:
            printf("Server said: ");
            printstr((char *)buf, n);
            break;
        case -1:
            keepRunning = false;
            break;
        }
    }

    printf("Disconnected from server, exiting...\n");
    close(sockfd);
    return 0;
}
