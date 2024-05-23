#include "common.h"
#include "serialize.h"
#include <pthread.h>

#define PORT "42069"
#define BACKLOG 5

typedef struct clientdata {
    int sockfd;
    struct sockaddr_storage addr;
    pthread_t thread;
    struct clientdata *prev;
    struct clientdata *next;
} clientdata;

int listener;
clientdata *clients = NULL; // Linked list
pthread_mutex_t clients_lock;

void interruptHandler() {
    // Close server listener socket
    close(listener);
}

/// Receive text and echo back to client.
void *clienthandler(void *arg) {
    // Move client data into function
    clientdata cd = *(clientdata *)arg;

    // Print new connection
    char clientaddress[32 + 1];
    getsockaddrstring((struct sockaddr *)&cd.addr, clientaddress,
                      sizeof(clientaddress));
    printf("New connection from %s!\n", clientaddress);

    char username[MAX_USERNAME];
    size_t usernameLength;
    bool hasRegistered = false;

    Buffer receivebuf;
    receivebuf.data = NULL;

    while (1) {
        if (receivebuf.data != NULL)
            free_buffer(&receivebuf);

        // Receive incoming message
        MsgType type;
        if ((type = msg_recv(cd.sockfd, &receivebuf)) == -1)
            break;

        if (!hasRegistered) {
            // Register user
            if (type == MSG_REGISTER) {
                RegisterMsg regm = deserialize_registermsg(&receivebuf);

                memcpy(username, regm.username, regm.usernameLength);
                usernameLength = regm.usernameLength;
                hasRegistered = true;
                printf("Client %s registered as ", clientaddress);
                printstr(username, usernameLength);
                printf("\n");

                free_deserialized_registermsg(&regm);
            }

            // Disregard any messages from client if not registered yet
            continue;
        }

        ChatMsg chatm;

        // Process message
        switch (type) {
        case MSG_CHAT:
            chatm = deserialize_chatmsg(&receivebuf);

            // Ensure user information is accurate
            free(chatm.username);
            chatm.username = calloc(usernameLength, sizeof(char));
            memcpy(chatm.username, username, usernameLength);
            chatm.usernameLength = usernameLength;

            // Echo message to all clients
            Buffer chatb;
            serialize_chatmsg(&chatb, chatm);
            pthread_mutex_lock(&clients_lock);

            for (clientdata *c = clients; c != NULL; c = c->next)
                msg_send(c->sockfd, MSG_CHAT, &chatb);

            pthread_mutex_unlock(&clients_lock);
            free_buffer(&chatb);

            free_deserialized_chatmsg(&chatm);
            break;
        default:
            break;
        }
    }

    // Disconnection
    printf("Connection closed by %s\n", clientaddress);
    close(cd.sockfd);

    // Remove from linked list
    pthread_mutex_lock(&clients_lock);
    clientdata *cd_arg = (clientdata *)arg;

    if (cd_arg->prev != NULL)
        cd_arg->prev->next = cd_arg->next;
    if (cd_arg->next != NULL)
        cd_arg->next->prev = cd_arg->prev;

    pthread_mutex_unlock(&clients_lock);
    free(cd_arg);

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
        printf("Failed to bind socket to address %s:%s: %s\n", hostaddress,
               PORT, strerror(errno));
        return 1;
    }
    freeaddrinfo(res);

    if (listen(listener, BACKLOG) != 0) {
        printf("Failed to listen for connections: %s\n", strerror(errno));
        return 1;
    }
    printf("Listening at %s:%s\n", hostaddress, PORT);

    signal(SIGINT, interruptHandler);
    pthread_mutex_init(&clients_lock, NULL);

    // Accept a new connection
    while (1) {
        struct sockaddr_storage clientaddr;
        socklen_t addrsize = sizeof(clientaddr);
        int clientfd =
            accept(listener, (struct sockaddr *)&clientaddr, &addrsize);

        if (clientfd < 0)
            break;

        // Create client data
        clientdata *data = malloc(sizeof(clientdata));
        data->sockfd = clientfd;
        data->addr = clientaddr;
        data->prev = NULL;
        data->next = NULL;

        // Update clients linked list
        if (clients != NULL) {
            clients->prev = data;
            data->next = clients;
        }
        clients = data;

        // Handle client in a new thread
        pthread_t clientthread;
        pthread_create(&clientthread, NULL, clienthandler, data);
        data->thread = clientthread;
    }

    pthread_mutex_destroy(&clients_lock);
    close(listener);
    return 0;
}
