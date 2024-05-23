#ifndef _MESSAGES_H
#define _MESSAGES_H

#include <stdint.h>

typedef enum {
    MSG_REGISTER,
    MSG_CHAT,
    MSG_DISCONNECT,
} MsgType;

#pragma pack(push, 1)

typedef struct {
    uint32_t usernameLength;
    char *username;
} RegisterMsg;

typedef struct {
    uint32_t usernameLength;
    char *username;
    uint32_t messageLength;
    char *message;
} ChatMsg;

typedef struct {
    uint32_t usernameLength;
    char *username;
} DisconnectMsg;

#pragma pack(pop)

#endif
