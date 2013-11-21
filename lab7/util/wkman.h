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
// Arguments:
// argc, argv: cmd line input
// pargs: partition arguments.
// acc: accumulator. Used in the synthesizer to collect worker output
// calc: Calculation. Run by workers on partition elements. Usually considered the "problem"
// part: Partitioner. Use to break up the problem into small chunks.
// synth: Synthesizer. Used to combine all of the small values returned by the workers.
// out: Output. Used by the synthesizer to output the result.

int runWkMan(int argc,
             char *argv[],
             void *pargs,
             void *acc,
             void *(*calc)(int, void *, size_t *),
             void (*part)(void *, void *),
             void (*synth)(void *, void *),
             void (*out)(void *));
