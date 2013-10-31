#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include "../util/macros.h"

static void worker(int size, int rank);
static void manager(int size);

int runWkMan(int argc, char *argv[]) {
    // Begin MPI
    MPI_Init(&argc, &argv);

    // Initialize within MPI
    int rank, size;

    // Get PID, num proc
    MPI_RANK_SIZE(&rank, &size);

    if (rank == 0) {
        manager(size);
    }
    else {
        worker(size, rank);

    }
    // Exit
    MPI_Finalize();
    return 0;
}

static void worker(int size, int rank) {
    int msg = 1;  
    void *buff;
    struct MPI_Status st;

    // Make space in buff
    buff = malloc(1 * sizeof(int));

    // Wait for message from manager
    MPI_OPEN_RECV(buff, &st);
    
    // When received from manager, print and return
    printf("Hello world! This is worker %d of %d.\n", rank, (size-1));
    fflush(stdout);

    // Send ok to manager
    MPI_OPEN_SEND((void *)&msg, 0);
    free(buff);

}

static void manager(int size) {
    int msg = 1;  
    int i;
    void *buff;
    struct MPI_Status st;

    // Make space in buff
    buff = malloc(1 * sizeof(int));

    // Send OK message to each worker, wait for OK response
    for (i = 1; i < size; i++) {
        MPI_OPEN_SEND((void *)&msg, i);
        MPI_OPEN_RECV(buff, &st);

    }
    free(buff);

}
