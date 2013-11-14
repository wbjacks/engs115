// Includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <mpi.h>
#include "../util/macros.h"
#include "../util/pqueue.h"

// Defines
#define WK_DONE 0
#define WK_KILL 1
#define WK_READY 2
#define SN_GO 3
#define WK_WAIT 4
#define NO_MSG -1

#define MAX_PKG_SIZE 100
#define WORKER_WAIT_TIME 0.1

// Prototypes
int runWkMan(int argc,
             char *argv[],
             void *pargs,
             void *acc,
             void *(*calc)(int rank, void *, size_t *size),
             void (*part)(void *, void *),
             void (*synth)(void *, void *),
             void (*out)(void *));
