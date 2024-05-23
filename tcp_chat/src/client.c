#include "common.h"
#include "serialize.h"
#include <poll.h>

volatile bool keepRunning = false;
int sockfd;

void interruptHandler() { keepRunning = false; }

int handle_incoming(int socket) {
    Buffer receivebuf;
    ChatMsg chatm;

    // Process message
    switch (msg_recv(socket, &receivebuf)) {
    case MSG_CHAT:
        chatm = deserialize_chatmsg(&receivebuf);

        printf("<");
        printstr(chatm.username, chatm.usernameLength);
        printf("> ");
        printstr(chatm.message, chatm.messageLength);
        printf("\n");

        free_deserialized_chatmsg(&chatm);
        break;
    case -1:
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

    // Removes trailing linebreak
    size_t contentLength = strlen(content) - 1;
    content[contentLength] = '\0';

    // Send message to server
    Buffer chatb;
    serialize_chatmsg(&chatb, (ChatMsg){0, "", contentLength, content});
    msg_send(sockfd, MSG_CHAT, &chatb);
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
    printf("Your username is: %s\n", username);

    // Register username with server
    Buffer regb;
    serialize_registermsg(&regb, (RegisterMsg){usernameLength, username});
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
