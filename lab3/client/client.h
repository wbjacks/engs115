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

#include "../util/user.h"

// Defines
#define MAX_INPUT_LENGTH 100

#define CF_MSG 0
#define CF_PING 1
#define CF_JOIN 2
#define CF_LEAVE 3
#define CF_WHO 4

