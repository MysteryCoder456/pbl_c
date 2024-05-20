#ifndef _SERIALIZE_H
#define _SERIALIZE_H

#include "messages.h"
#include <stdlib.h>

typedef struct {
    void *data;
    size_t next;
} Buffer;

void free_buffer(Buffer *buffer);

void serialize_str(Buffer *buffer, char *str, size_t strlen);
void serialize_int(Buffer *buffer, uint32_t integer);

void serialize_registermsg(Buffer *buffer, RegisterMsg msg);
RegisterMsg deserialize_registermsg(const void *data);
void free_deserialized_registermsg(RegisterMsg *msg);

void serialize_chatmsg(Buffer *buffer, ChatMsg msg);
ChatMsg deserialize_chatmsg(const void *data);
void free_deserialized_chatmsg(ChatMsg *msg);

#endif
