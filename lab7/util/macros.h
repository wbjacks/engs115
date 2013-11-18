// Macros
// MPI
#define MPI_OPEN_SEND(b, dest, size) MPI_Send(b, size, MPI_CHAR, dest, 0, \
    MPI_COMM_WORLD)
#define MPI_OPEN_RECV(b, st, size) MPI_Recv(b, size, MPI_INT, MPI_ANY_SOURCE, \
    MPI_ANY_TAG, MPI_COMM_WORLD, st)
#define MPI_RANK_SIZE(r, s) \
    MPI_Comm_rank(MPI_COMM_WORLD, r);\
    MPI_Comm_size(MPI_COMM_WORLD, s)

// Args
#define CHECK_ARGC(n) if (argc != n) { \
    fprintf(stderr, "Error: Bad arguments. Exiting...\n");
    return EXIT_FAILURE; }

// Standard defines
#define TRUE 1
#define FALSE 0
