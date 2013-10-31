#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include "../util/macros.h"

int runWkMan(int argc, char *argv[]);
int runTkn(int argc, char *argv[]);
int runRevTkn(int argc, char *argv[]);

int main(int argc, char *argv[]) {
    // Arguments
    switch (atoi(argv[1])) {
        case 1: runWkMan(argc, argv); break;

        case 2: runTkn(argc, argv); break;

        case 3: runRevTkn(argc, argv); break;

        default:
            fprintf(stderr, "Error: bad input. Exiting...\n");
            break;

    }
    return EXIT_SUCCESS;

}

int runWkMan(int argc, char *argv[]) {
    // Begin MPI
    MPI_Init(&argc, &argv);

    // Initialize within MPI
    int rank, size;
    int msg = 1;  
    int i;
    void *buff;
    struct MPI_Status st;

    // Make space in buff
    buff = malloc(1 * sizeof(int));

    // Get PID, num proc
    MPI_RANK_SIZE(&rank, &size);

    if (rank == 0) {
        // Manager
        // Send OK message to each worker, wait for OK response
        for (i = 1; i < size; i++) {
            MPI_OPEN_SEND((void *)&msg, i);
            MPI_OPEN_RECV(buff, st);

        }
    }
    else {
        // Worker
        // Wait for message from manager
        MPI_OPEN_RECV(buff, st);
        
        // When received from manager, print and return
        printf("Hello world! This is worker %d of %d.\n", rank, (size-1));
        fflush(stdout);

        // Send ok to manager
        MPI_OPEN_SEND((void *)&msg, 0);

    }
    // Exit
    free(buff);
    MPI_Finalize();
    return 0;
}

int runTkn(int argc, char *argv[]) {
    // Begin MPI
    MPI_Init(&argc, &argv);

    // Initialize stuff
    int send_to, rank, size;
    void *buff;
    struct MPI_Status st;
    buff = malloc(1 * sizeof(int));

    MPI_RANK_SIZE(&rank, &size);
    send_to = rank+1;

    if (rank != 0) {
        // Wait for recieve if not the first
        MPI_OPEN_RECV(buff, st);

    }
    // Got message, do that print thing
    printf("Hello world! This is link %d of %d.\n", rank, size);
    fflush(stdout);

    if (rank != (size - 1)) {
        // Send to next if not the last
        memset(buff, 1, sizeof(int));
        MPI_OPEN_SEND(buff, send_to);

    }
    free(buff);
    MPI_Finalize();
    return 0;

}

int runRevTkn(int argc, char *argv[]) {
    // Begin MPI
    MPI_Init(&argc, &argv);

    // Initialize stuff
    int send_to, rank, size;
    void *buff;
    struct MPI_Status st;
    buff = malloc(1 * sizeof(int));

    MPI_RANK_SIZE(&rank, &size);
    send_to = rank-1;

    if (rank != (size - 1)) {
        // Wait for recieve if not the first
        MPI_OPEN_RECV(buff, st);

    }
    // Got message, do that print thing
    printf("Hello world! This is link %d of %d.\n", rank, size);
    fflush(stdout);

    if (rank != 0) {
        // Send to next if not the last
        memset(buff, 1, sizeof(int));
        MPI_OPEN_SEND(buff, send_to);

    }
    free(buff);
    MPI_Finalize();
    return 0;

}
