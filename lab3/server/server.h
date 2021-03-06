// Includes
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>

#include "../util/pqueue.h"
#include "../util/user.h"

// Defines
#define MAX_MSG_LENGTH 100
#define MAX_TOK 99
#define MAX_INPUT_LENGTH 100

#define NUM_PARSERS 1
#define NUM_DISPATCHERS 1

#define PARSER_SLEEP_TIME 1
#define DISPATCHER_SLEEP_TIME 1

#define CF_MSG 0
#define CF_PING 1
#define CF_JOIN 2
#define CF_LEAVE 3
#define CF_WHO 4

// Structures

struct __chat_command {
    int type;
    void *data;
    ChatUser_t issuer;

    // Might be able to take these out, as they are in ChatUser_t
    struct sockaddr_storage src;
    socklen_t src_len;

};
typedef struct __chat_command ChatCommand_t;

struct __receiver_input {
    struct addrinfo *info;

};
typedef struct __receiver_input ReceiverInput_t;

struct __message_queue_element {
    char *str;
    struct sockaddr_storage src;
    socklen_t src_len;

};
typedef struct __message_queue_element MsgQueueElement_t;
