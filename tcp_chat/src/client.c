#include "common.h"
#include <poll.h>

volatile bool keepRunning = false;
int sockfd;

void interruptHandler() { keepRunning = false; }

int handle_incoming(int socket) {
    Buffer receivebuf;
    union {
        RegisterMsg regis;
        ChatMsg chat;
        DisconnectMsg disc;
    } msg;

    // Process message
    switch (msg_recv(socket, &receivebuf)) {
    case MSG_REGISTER:
        msg.regis = deserialize_registermsg(&receivebuf);

        // TODO: ASCII art of username
        printf("\n");
        printstr(msg.regis.username, msg.regis.usernameLength);
        printf(" HAS JOINED THE CHAT\n\n");

        free_deserialized_registermsg(&msg.regis);
        break;

    case MSG_CHAT:
        msg.chat = deserialize_chatmsg(&receivebuf);

        printf("<");
        printstr(msg.chat.username, msg.chat.usernameLength);
        printf("> ");
        printstr(msg.chat.message, msg.chat.messageLength);
        printf("\n");

        free_deserialized_chatmsg(&msg.chat);
        break;

    case MSG_DISCONNECT:
        msg.disc = deserialize_disconnectmsg(&receivebuf);

        printf("\n");
        printstr(msg.regis.username, msg.regis.usernameLength);
        printf(" HAS LEFT THE CHAT\n\n");

        free_deserialized_disconnectmsg(&msg.disc);
        break;

    case -1:
        printf("Failed to receive message: %s\n", strerror(errno));
        keepRunning = false;
        return -1;
    }

    free_buffer(&receivebuf);
    return 0;
}

void handle_stdin(int socket) {
    // Accept message content from user
    char content[256 + 1];
    fgets(content, sizeof(content), stdin);
    printf("\033[A                             \033[A\n");

    // Removes trailing linebreak
    size_t contentLength = strlen(content) - 1;
    content[contentLength] = '\0';

    // Send message to server
    Buffer chatb;
    if (serialize_chatmsg(&chatb, (ChatMsg){0, "", contentLength, content}) > 0)
        msg_send(sockfd, MSG_CHAT, &chatb);
    else
        printf("Failed to serialize chat message: %s\n", strerror(errno));
}

int main() {
    // Accept server address and port
    char *serveraddr;
    char *serverport;
    size_t serverlen;

    // HACK: we can do better
    char userserveraddr[64 + 2] = "127.0.0.1";
    printf("Enter server address (127.0.0.1): ");
    fgets(userserveraddr, 64 + 2, stdin);
    serverlen = strlen(userserveraddr) - 1;
    userserveraddr[serverlen] = '\0';
    if (serverlen <= 0)
        serveraddr = "127.0.0.1";
    else
        serveraddr = userserveraddr;

    // HACK: we can do better
    char userserverport[5 + 2];
    printf("Enter server port (42069): ");
    fgets(userserverport, 5 + 2, stdin);
    serverlen = strlen(userserverport) - 1;
    userserverport[serverlen] = '\0';
    if (serverlen <= 0)
        serverport = "42069";
    else
        serverport = userserverport;

    // Generate server address info
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(serveraddr, serverport, &hints, &res) != 0) {
        printf("Failed to get server address info: %s\n", strerror(errno));
        return 1;
    }

    // Create socket and connect to server
    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (connect(sockfd, res->ai_addr, res->ai_addrlen) != 0) {
        printf("Failed to connect to server: %s\n", strerror(errno));
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
    // printf("Your username is: %s\n", username);

    // Register username with server
    Buffer regb;
    if (serialize_registermsg(&regb, (RegisterMsg){usernameLength, username}) <=
        0) {
        printf("Failed to serialize register message: %s\n", strerror(errno));
        close(sockfd);
        return 1;
    }
    msg_send(sockfd, MSG_REGISTER, &regb);
    free_buffer(&regb);

    while (keepRunning) {
        struct pollfd poll_fds[2];

        // stdin
        poll_fds[0].fd = STDIN_FILENO;
        poll_fds[0].events = POLLIN;

        // socket
        poll_fds[1].fd = sockfd;
        poll_fds[1].events = POLLIN;

        // Begin polling
        if (poll(poll_fds, 2, -1) == -1)
            break;

        // Check socket disconnection
        if ((poll_fds[1].revents & POLLHUP) == POLLHUP)
            break;

        // Send messages to server
        if ((poll_fds[0].revents & POLLIN) == POLLIN)
            handle_stdin(sockfd);

        // Receive incoming server messages
        if ((poll_fds[1].revents & POLLIN) == POLLIN &&
            handle_incoming(sockfd) == -1)
            break;
    }

    printf("Disconnected from server, exiting...\n");
    close(sockfd);
    return 0;
}
