#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>
#include "../util/macros.h"

int runWkMan(int rank, int size) {
    if (rank == 0) {
        manager(size);

    }
    else {
        worker(rank, size);

    }
}

static void worker(int rank, int size) {
    void *buff;
    struct MPI_Status st;
    buff = malloc(1 * sizeof(int));

    // Wait for message from manager
    MPI_OPEN_RECV(buff, &st);
    
    // When received from manager, print and return
    printf("Hello world! This is worker %d of %d.\n", rank, (size-1));
    fflush(stdout);

    // Free and return
    free(buff);

}

static void manager(int size) {
    void *buff;
    int msg = 1;  
    int i;

    // Send OK message to each worker
    for (i = 1; i < size; i++) {
        MPI_OPEN_SEND((void *)&msg, i);

    }
}
