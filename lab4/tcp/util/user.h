// File for implementing chat users

// Includes
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <pthread.h>

// Defines
#define MAX_ALIAS_SIZE 91
#define MAX_ROOM_NAME_SIZE 100
#define MAX_USER_ID 999.0
#define MIN_USER_ID 0.0

// Structs
struct __chat_user {
    int id;
    char alias[MAX_ALIAS_SIZE]; // Be sure to enfore 90 char limit
    int sub_socket;

};

typedef struct __chat_user ChatUser_t;

struct __chat_room {
    int id;
    char name[MAX_ROOM_NAME_SIZE];
    //struct sockaddr_in address;
    int user_count;
    void *qp; // List of users
    pthread_mutex_t lock;

};

typedef struct __chat_room ChatRoom_t;

// Prototypes
ChatUser_t *create_user(void);
