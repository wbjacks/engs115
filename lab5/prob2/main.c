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

    // Check size and run stuff
    if (size > 1) {
        // Arguments
        switch (atoi(argv[1])) {
            case 1: runWkMan(rank, size); break;

            case 2:

            case 3:

            default:
                fprintf(stderr, "Error: bad input. Exiting...");
                break;

        }
    }
    // Exit
    MPI_Finalize();
    return EXIT_SUCCESS;

}
