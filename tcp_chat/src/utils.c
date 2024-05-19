#include "utils.h"
#include <arpa/inet.h>
#include <stdio.h>

#define PACKET_LEN 1024

void getsockaddrstring(struct sockaddr *addr, char *str, size_t str_size) {
    if (addr->sa_family == AF_INET) {
        struct sockaddr_in *addr4 = (struct sockaddr_in *)addr;
        inet_ntop(AF_INET, &addr4->sin_addr, str, str_size);
    } else {
        struct sockaddr_in6 *addr6 = (struct sockaddr_in6 *)addr;
        inet_ntop(AF_INET6, &addr6->sin6_addr, str, str_size);
    }
}

void printstr(char *str, size_t len) {
    for (int i = 0; i < len; i++)
        printf("%c", str[i]);
    printf("\n");
}

size_t sendall(int socket, const void *buffer, size_t length, int flags) {
    size_t bytesSent = 0;

    while (bytesSent < length) {
        size_t bytesLeft = length - bytesSent;
        size_t bytesToSend = bytesLeft < PACKET_LEN ? bytesLeft : PACKET_LEN;

        int ret = send(socket, (char *)buffer + bytesSent, bytesToSend, flags);

        if (ret <= 0)
            return ret;
        bytesSent += ret;
    }

    return bytesSent;
}

void msg_send(int socket, message_type type, const void *buffer,
              message_size length) {
    // Message `length` is cast to `unsigned long long` and used between
    // `msg_send()` and `msg_recv()`. The functions' users only ever
    // deal with the `message_size` type.
    unsigned long long networkLength = htonll((unsigned long long)length);

    // Send message length, type and data
    sendall(socket, &networkLength, sizeof(networkLength), 0);
    sendall(socket, &type, sizeof(type), 0);
    sendall(socket, buffer, length, 0);
}

message_type msg_recv(int socket, void **buffer, message_size *length) {
    // Length
    unsigned long long networkLength;
    if (recv(socket, &networkLength, sizeof(networkLength), 0) <= 0)
        return -1;
    message_size msgLength = (message_size)ntohll(networkLength);
    *length = msgLength;

    // Type
    message_type type;
    if (recv(socket, &type, sizeof(type), 0) <= 0)
        return -1;

    // Data
    void *msg = malloc(msgLength);
    if (recv(socket, msg, msgLength, 0) <= 0) {
        free(msg);
        return -1;
    }
    *buffer = msg;

    return type;
}
