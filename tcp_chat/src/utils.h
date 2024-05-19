#ifndef _UTILS_H
#define _UTILS_H

#include <stdlib.h>
#include <sys/socket.h>

typedef uint32_t message_size;
typedef int8_t message_type;

void getsockaddrstring(struct sockaddr *addr, char *str, size_t str_size);
void printstr(char *str, size_t len);

/// Ensures that `length` number of bytes from `buffer` are sent to `socket`.
size_t sendall(int socket, const void *buffer, size_t length, int flags);

/// Sends `length` bytes to `socket`, conforming to the self-imposed protocol.
void msg_send(int socket, message_type type, const void *buffer,
              message_size length);

/// Receives a protocol conforming message from `socket`, mutating
/// `buffer` and `length` point to the dynamically allocated message data
/// and length of the data received.
message_type msg_recv(int socket, void **buffer, message_size *length);

#endif
