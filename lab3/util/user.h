// File for implementing chat users

// Includes
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>

// Defines
#define MAX_ALIAS_SIZE 91

// Structs
struct __chat_user {
    int id;
    char alias[MAX_ALIAS_SIZE]; // Be sure to enfore 90 char limit

};

typedef struct __chat_user ChatUser_t;

struct __chat_room {
    int id;
    struct sockaddr_in address;
    int user_count;
    void *qp; // List of users

};

typedef struct __chat_room ChatRoom_t;

// Prototypes
ChatUser_t *create_user(void);
