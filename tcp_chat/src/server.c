#include "common.h"
#include <pthread.h>
#include <stdbool.h>

#define PORT "8000"
#define BACKLOG 5

int listener;

void interruptHandler() {
    // Close server listener socket
    close(listener);
}

/// Used to pass data into `clienthandler`.
typedef struct {
    int sockfd;
    struct sockaddr_storage addr;
} clientdata;

/// Receive text and echo back to client.
void *clienthandler(void *arg) {
    // Move client data into function
    clientdata cd = *(clientdata *)arg;
    free(arg);

    // Print new connection
    char clientaddress[32 + 1];
    getsockaddrstring((struct sockaddr *)&cd.addr, clientaddress,
                      sizeof(clientaddress));
    printf("New connection from %s!\n", clientaddress);

    char username[MAX_USERNAME];
    bool hasRegistered = false;
    void *buf = NULL;

    while (1) {
        if (buf != NULL)
            free(buf);

        // Receive incoming message
        message_size n;
        MsgType type;
        if ((type = msg_recv(cd.sockfd, (void **)&buf, &n)) == -1)
            break;

        if (!hasRegistered) {
            // Register user
            if (type == MSG_REGISTER) {
                memcpy(username, buf, n > MAX_USERNAME ? MAX_USERNAME : n);
                hasRegistered = true;
                printf("Client %s registered as '%s'\n", clientaddress,
                       username);
            }

            // Disregard any messages from client if not registered yet
            continue;
        }

        // Process message
        switch (type) {
        case MSG_CHAT:
            printf("<%s> ", username);
            printstr(buf, n);
            msg_send(cd.sockfd, 0, "ok bozo", 7);
            break;
        default:
            break;
        }
    }

    // Print disconnection
    printf("Connection closed by %s\n", clientaddress);

    close(cd.sockfd);
    return NULL;
}

int main() {
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if (getaddrinfo(NULL, PORT, &hints, &res) != 0) {
        printf("Failed to get address info\n");
        return 1;
    }

    char hostaddress[32 + 1];
    getsockaddrstring(res->ai_addr, hostaddress, sizeof(hostaddress));

    // Create, bind and listen on a socket

    listener = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

    if (bind(listener, res->ai_addr, res->ai_addrlen) != 0) {
        printf("Failed to bind socket to address %s:%s\n", hostaddress, PORT);
        return 1;
    }
    freeaddrinfo(res);

    if (listen(listener, BACKLOG) != 0) {
        printf("Failed to listen for connections\n");
        return 1;
    }
    printf("Listening at %s:%s\n", hostaddress, PORT);

    signal(SIGINT, interruptHandler);

    // Accept a new connection
    while (1) {
        struct sockaddr_storage clientaddr;
        socklen_t addrsize = sizeof(clientaddr);
        int clientfd =
            accept(listener, (struct sockaddr *)&clientaddr, &addrsize);

        if (clientfd < 0)
            break;

        // Handle client in a new thread

        clientdata *data = malloc(sizeof(clientdata));
        data->sockfd = clientfd;
        data->addr = clientaddr;

        pthread_t clientthread;
        pthread_create(&clientthread, NULL, clienthandler, data);

        // TODO: store client threads
    }

    close(listener);
    return 0;
}
