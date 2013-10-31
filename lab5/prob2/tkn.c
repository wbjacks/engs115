#include "tkn.h"

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
        MPI_OPEN_RECV(buff, &st);

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
