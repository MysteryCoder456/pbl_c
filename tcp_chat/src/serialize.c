#include "serialize.h"
#include "messages.h"
#include <string.h>

void free_buffer(Buffer *buffer) { free(buffer->data); }

// ————— Primitives —————

void serialize_str(Buffer *buffer, char *str, size_t strlen) {
    // TODO: Determine whether buffer has been allocated

    memcpy((char *)buffer->data + buffer->next, str, strlen);
    buffer->next += strlen;
}

char *deserialize_str(const void *data, size_t strlen) {
    char *str = calloc(strlen, sizeof(char));
    memcpy(str, (char *)data, strlen);
    return str;
}

void serialize_int(Buffer *buffer, uint32_t integer) {
    // TODO: Determine whether buffer has been allocated

    uint32_t networkInt = htonl(integer);
    memcpy((char *)buffer->data + buffer->next, &networkInt,
           sizeof(networkInt));
    buffer->next += sizeof(networkInt);
}

uint32_t deserialize_int(const void *data) {
    uint32_t networkInt;
    memcpy(&networkInt, data, sizeof(networkInt));
    return ntohl(networkInt);
}

// ————— RegisterMsg —————

void serialize_registermsg(Buffer *buffer, RegisterMsg msg) {
    buffer->data = malloc(msg.usernameLength + sizeof(msg.usernameLength));
    buffer->next = 0;

    serialize_int(buffer, msg.usernameLength);
    serialize_str(buffer, msg.username, msg.usernameLength);
}

RegisterMsg deserialize_registermsg(const void *data) {
    RegisterMsg msg;
    size_t next = 0;

    // TODO: Replace with `Buffer`

    msg.usernameLength = deserialize_int((char *)data + next);
    next += sizeof(msg.usernameLength);

    msg.username = deserialize_str((char *)data + next, msg.usernameLength);
    next += msg.usernameLength;

    return msg;
}

void free_deserialized_registermsg(RegisterMsg *msg) { free(msg->username); }

// ————— ChatMsg —————

void serialize_chatmsg(Buffer *buffer, ChatMsg msg) {
    buffer->data = malloc(msg.usernameLength + sizeof(msg.usernameLength) +
                          msg.messageLength + sizeof(msg.messageLength));
    buffer->next = 0;

    serialize_int(buffer, msg.usernameLength);
    serialize_str(buffer, msg.username, msg.usernameLength);
    serialize_int(buffer, msg.messageLength);
    serialize_str(buffer, msg.message, msg.messageLength);
}

ChatMsg deserialize_chatmsg(const void *data) {
    ChatMsg msg;
    size_t next = 0;

    // TODO: Replace with `Buffer`

    msg.usernameLength = deserialize_int((char *)data + next);
    next += sizeof(msg.usernameLength);

    msg.username = deserialize_str((char *)data + next, msg.usernameLength);
    next += msg.usernameLength;

    msg.messageLength = deserialize_int((char *)data + next);
    next += sizeof(msg.messageLength);

    msg.message = deserialize_str((char *)data + next, msg.messageLength);
    next += msg.messageLength;

    return msg;
}

void free_deserialized_chatmsg(ChatMsg *msg) {
    free(msg->username);
    free(msg->message);
}
