// Includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include "../util/macros.h"

// Defines
#define WK_DONE 0
#define WK_KILL 1
#define NO_MSG -1

#define MAX_PKG_SIZE 100

// Prototypes
int runWkMan(int argc, char *argv[]);
