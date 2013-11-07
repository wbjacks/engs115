// Includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include "../util/macros.h"
#include "../util/pqueue.h"

// Defines
#define WK_DONE 0
#define WK_KILL 1
#define WK_READY 2
#define SN_GO 3
#define NO_MSG -1

#define MAX_PKG_SIZE 100

// Prototypes
int runWkMan(int argc,
             char *argv[],
             void *(*part_args)(void *),
             void *(*acc)(void),
             void *(*calc)(void *, size_t *size),
             void (*part)(void *),
             void (*synth)(void *, void *),
             void (*out)(void *));
