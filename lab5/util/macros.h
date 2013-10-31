#include <mpi.h>

// Macros
#define MPI_OPEN_SEND(b, dest) MPI_Send(b, sizeof(b), MPI_CHAR, dest, 0, \
    MPI_COMM_WORLD)
#define MPI_OPEN_RECV(b, st) MPI_Recv(b, sizeof(b), MPI_INT, MPI_ANY_SOURCE, \
    MPI_ANY_TAG, MPI_COMM_WORLD, st)
#define MPI_RANK_SIZE(r, s) \
    MPI_Comm_rank(MPI_COMM_WORLD, r);\
    MPI_Comm_size(MPI_COMM_WORLD, s);

