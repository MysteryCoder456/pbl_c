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

char *deserialize_str(Buffer *buffer, size_t strlen) {
    char *str = calloc(strlen, sizeof(char));
    memcpy(str, (char *)buffer->data + buffer->next, strlen);

    buffer->next += strlen;
    return str;
}

void serialize_int(Buffer *buffer, uint32_t integer) {
    // TODO: Determine whether buffer has been allocated

    uint32_t networkInt = htonl(integer);
    memcpy((char *)buffer->data + buffer->next, &networkInt,
           sizeof(networkInt));
    buffer->next += sizeof(networkInt);
}

uint32_t deserialize_int(Buffer *buffer) {
    uint32_t networkInt;
    memcpy(&networkInt, (char *)buffer->data + buffer->next,
           sizeof(networkInt));

    buffer->next += sizeof(networkInt);
    return ntohl(networkInt);
}

// ————— RegisterMsg —————

void serialize_registermsg(Buffer *buffer, RegisterMsg msg) {
    buffer->data = malloc(msg.usernameLength + sizeof(msg.usernameLength));
    buffer->next = 0;

    serialize_int(buffer, msg.usernameLength);
    serialize_str(buffer, msg.username, msg.usernameLength);
}

RegisterMsg deserialize_registermsg(Buffer *buffer) {
    RegisterMsg msg;
    buffer->next = 0;

    msg.usernameLength = deserialize_int(buffer);
    msg.username = deserialize_str(buffer, msg.usernameLength);

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

ChatMsg deserialize_chatmsg(Buffer *buffer) {
    ChatMsg msg;
    buffer->next = 0;

    msg.usernameLength = deserialize_int(buffer);
    msg.username = deserialize_str(buffer, msg.usernameLength);
    msg.messageLength = deserialize_int(buffer);
    msg.message = deserialize_str(buffer, msg.messageLength);

    return msg;
}

void free_deserialized_chatmsg(ChatMsg *msg) {
    free(msg->username);
    free(msg->message);
}
