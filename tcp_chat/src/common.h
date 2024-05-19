#ifndef _COMMON_H
#define _COMMON_H

#include <netdb.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "utils.h"

#define MAX_USERNAME 16

typedef enum {
    MSG_REGISTER,
    MSG_CHAT,
} MsgType;

#endif
