#include "common.h"
#include <pthread.h>

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

    while (1) {
        // Receive incoming message
        char *buf;
        message_size n;
        if (msg_recv(cd.sockfd, (void **)&buf, &n) == -1)
            break;

        // Process message
        printf("Client %s said: ", clientaddress);
        printstr(buf, n);

        // Free message buffer
        free(buf);

        // Respond to client
        msg_send(cd.sockfd, 0, "ok bozo", 7);
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
