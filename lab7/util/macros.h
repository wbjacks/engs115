// Macros
// MPI
#define MPI_OPEN_SEND(b, dest, size) MPI_Send(b, size, MPI_CHAR, dest, 0, \
    MPI_COMM_WORLD)
#define MPI_OPEN_RECV(b, st, size) MPI_Recv(b, size, MPI_INT, MPI_ANY_SOURCE, \
    MPI_ANY_TAG, MPI_COMM_WORLD, st)

#define MPI_MSG_SEND(b, dest, size) MPI_Send(b, size, MPI_CHAR, dest, 99, \
    MPI_COMM_WORLD)
#define MPI_MSG_RECV(b, st, size) MPI_Recv(b, size, MPI_INT, MPI_ANY_SOURCE, \
    99, MPI_COMM_WORLD, st)

#define MPI_TKN_SEND(b, dest, size) MPI_Send(b, size, MPI_CHAR, dest, 42, \
    MPI_COMM_WORLD)
#define MPI_TKN_RECV(b, st, size) MPI_Recv(b, size, MPI_INT, MPI_ANY_SOURCE, \
    42, MPI_COMM_WORLD, st)

#define MPI_RANK_SIZE(r, s) \
    MPI_Comm_rank(MPI_COMM_WORLD, r);\
    MPI_Comm_size(MPI_COMM_WORLD, s)

// Other
#define SAFE_FREE(b) free(buff); buff = NULL

// Args
#define CHECK_ARGC(n_val) if (argc != n_val) { \
    fprintf(stderr, "Error: Bad arguments. Exiting...\n");\
    return EXIT_FAILURE; }

// Standard defines
#define TRUE 1
#define FALSE 0
