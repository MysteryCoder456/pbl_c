#ifndef _SERIALIZE_H
#define _SERIALIZE_H

#include "messages.h"
#include <stdlib.h>

typedef struct {
    void *data;
    size_t next;
} Buffer;

void free_buffer(Buffer *buffer);

// ————— Primitives —————
/*
void serialize_str(Buffer *buffer, char *str, size_t strlen);
char *deserialize_str(Buffer *buffer, size_t strlen);

void serialize_int(Buffer *buffer, uint32_t integer);
uint32_t deserialize_int(Buffer *buffer);
*/

// ————— RegisterMsg —————
size_t serialize_registermsg(Buffer *buffer, RegisterMsg msg);
RegisterMsg deserialize_registermsg(Buffer *buffer);
void free_deserialized_registermsg(RegisterMsg *msg);

// ————— ChatMsg —————
size_t serialize_chatmsg(Buffer *buffer, ChatMsg msg);
ChatMsg deserialize_chatmsg(Buffer *buffer);
void free_deserialized_chatmsg(ChatMsg *msg);

// ————— DisconnectMsg —————
size_t serialize_disconnectmsg(Buffer *buffer, DisconnectMsg msg);
DisconnectMsg deserialize_disconnectmsg(Buffer *buffer);
void free_deserialized_disconnectmsg(DisconnectMsg *msg);

#endif
