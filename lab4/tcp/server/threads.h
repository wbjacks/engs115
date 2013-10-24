// Includes
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>

#include "commands.h"

// Defines
#define PARSER_SLEEP_TIME 1
#define DISPATCHER_SLEEP_TIME 1

#define CF_MSG 0
#define CF_PING 1
#define CF_JOIN 2
#define CF_LEAVE 3
#define CF_WHO 4

#define LISTEN_BACKLOG 20

#define MAX_MSG_LENGTH 100
#define MAX_TOK 99

#define NUM_PARSERS 1
#define NUM_DISPATCHERS 1

// Structures

struct __receiver_input {
    int sub_socket;

};
typedef struct __receiver_input ReceiverInput_t;

struct __message_queue_element {
    char *str;
    int sub_socket;

};
typedef struct __message_queue_element MsgQueueElement_t;

// Prototypes
void connector(struct addrinfo *info);
void *receiver(void *pt);
void *parser(void *pt);
void *dispatcher(void *pt);

// Globals
void *msg_queue;
void *task_queue;
void *connection_queue;
int sockfd;
