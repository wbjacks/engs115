#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char *argv[]) {
    // Begin MPI
    MPI_Init(&argc, &argv);

    // Initialize within MPI
    int rank, size;

    // Get PID, num proc
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Be polite
    printf("Hello world! This is process %d of %d!\n", rank, size);
    fflush(stdout);

    // Exit
    MPI_Finalize();
    return EXIT_SUCCESS;

}
