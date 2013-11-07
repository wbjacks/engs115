#include "wkman.h"

// Internal Structures
struct __wk_data {
    int msg;
    size_t pkg_size;
    void *pkg;

};
typedef struct __wk_data WkData_t;

// Statics
static void worker(int size, int rank, void *(*calc)(void *, size_t *size));
static void manager(int size, void (*partitioner)(void *qp), void *(*part_args)(void *));
static void synthesizer(void *(*acc)(void), void (*synth)(void *, void *), void (*out)(void *));
static void *pack(WkData_t *package);
static WkData_t *unpack(void *buff);

int runWkMan(int argc,
             char *argv[],
             void *(*part_args)(void *),
             void *(*acc)(void),
             void *(*calc)(void *, size_t *size),
             void (*part)(void *),
             void (*synth)(void *, void *),
             void (*out)(void *))
{
    // Begin MPI
    MPI_Init(&argc, &argv);

    // Initialize within MPI
    int rank, size;

    // Get PID, num proc
    MPI_RANK_SIZE(&rank, &size);

    // Launch
    if (rank == 0)
        manager(size, part, part_args);

    else if(rank == (size -1))
        synthesizer(acc, synth, out);

    else 
        worker(size, rank, calc);
 
    // Exit
    MPI_Finalize();
    return 0;

}

static void worker(int size, int rank, void *(*calc)(void *, size_t *size)) {
    int kill = FALSE;
    void *buff;
    void *r_buff;
    void *ret_pkg;
    size_t ret_pkg_size;
    MPI_Status st;
    WkData_t *data;
    WkData_t ret_data;

    r_buff = malloc(MAX_PKG_SIZE);

    // Send initial ready message
    ret_data.msg = WK_READY;
    ret_data.pkg_size = sizeof(int);
    ret_data.pkg = malloc(sizeof(int));
    memcpy(ret_data.pkg, &rank, sizeof(int));
    buff = pack(&ret_data);
    MPI_OPEN_SEND(buff, 0, MAX_PKG_SIZE);
    if (buff) free(buff);

    // Loop until kill is set
    while (!kill) {
        // Wait for message from manager, unpack, clear buffer
        MPI_OPEN_RECV(r_buff, &st, MAX_PKG_SIZE);
        data = unpack(r_buff);
        if (!data) {
            fprintf(stderr, "Error worker %d!\n", rank);
            return;

        }
        memset(r_buff, 0, MAX_PKG_SIZE);

        // Recieved message... open
        if (data->msg != NO_MSG) {
            switch (data->msg) {
                case WK_KILL: kill = TRUE; break;
                default:
                    fprintf(stderr, "Warning: Bad message received at worker %d.\n", rank);
                    break;

            }
        }
        else {
            // If message is empty, throw pkg at the supplied task
            ret_pkg = calc(data->pkg, &ret_pkg_size);

            // Pack data, send to sync 
            ret_data.msg = NO_MSG;
            ret_data.pkg_size = ret_pkg_size;
            ret_data.pkg = ret_pkg;
            buff = pack(&ret_data);

            // Send data back
            MPI_OPEN_SEND(buff, (size-1), MAX_PKG_SIZE);
            if (buff) free(buff);

            // Send WK_READY to manager
            ret_data.msg = WK_READY;
            ret_data.pkg_size = sizeof(int);
            ret_data.pkg = malloc(sizeof(int));
            memcpy(ret_data.pkg, &rank, sizeof(int));
            buff = pack(&ret_data);
            MPI_OPEN_SEND(buff, 0, MAX_PKG_SIZE);
            if (buff) free(buff);

            // Cleanup
            memset(ret_pkg, 0, ret_pkg_size);

        }
        if (data) free(data);
    
    }
    // Kill has been set... clean up and die in peace
    ret_data.msg = WK_DONE;
    ret_data.pkg_size = 0;
    ret_data.pkg = NULL;
    buff = pack(&ret_data);
    MPI_OPEN_SEND(buff, 0, MAX_PKG_SIZE);
    return;

}

static void manager(int size, void (*partitioner)(void *qp), void *(*part_args)(void *)) {
    int done_workers = 0;
    void *buff, *r_buff;
    void *qp;
    void *qval;
    size_t qval_size;
    MPI_Status st;
    WkData_t *data;
    WkData_t ret_data;

    r_buff = malloc(MAX_PKG_SIZE);

    // Open shit
    qp = pqopen();

    // Run partitioner-> REACH do this in a thread
    partitioner(part_args(qp));
    //printf("Partitioned!\n");

    // Loop until workers done
    while (done_workers < (size-2)) {
        //printf("dead workers: %d.\n", done_workers);

        // Receive message, check
        MPI_OPEN_RECV(r_buff, &st, MAX_PKG_SIZE);
        data = unpack(r_buff);
        if (!data) {
            fprintf(stderr, "Error in manager!\n");
            return;

        }
        memset(r_buff, 0, MAX_PKG_SIZE);
        if (data->msg == WK_DONE) 
            done_workers++;

        else {
            // Grab value from queue
            qval = pqget(qp, &qval_size);
            if (!qval) {
                // If queue value is NULL, send WK_KILL
                //printf("Sending Kill to %d..\n", *(int *)data->pkg);
                ret_data.msg = WK_KILL;
                ret_data.pkg_size = 0;
                ret_data.pkg = NULL;

            }
            else {
                // Else, send partition
                //printf("Size from queue: %d\n", (int)qval_size);
                ret_data.msg = NO_MSG;
                ret_data.pkg_size = qval_size;
                ret_data.pkg = qval;
                //if (qval) free(qval);

            }
            buff = pack(&ret_data);
            MPI_OPEN_SEND(buff, *((int *)data->pkg), MAX_PKG_SIZE);
            //printf("buff value before freeing: %p.\n", buff);
            if (buff) free(buff);

        }
        if (data) free(data);

    }
    // Send sync_ok
    ret_data.msg = SN_GO;
    ret_data.pkg_size = 0;
    ret_data.pkg = NULL;
    buff = pack(&ret_data);
    MPI_OPEN_SEND(buff, (size-1), MAX_PKG_SIZE);
    if (buff) free(buff);

    // Cleanup and return
    pqclose(qp);
    return;

}

static void synthesizer(void *(*acc)(void), void (*synth)(void *, void *), void (*out)(void *)) {
    // Manages program output
    void *buff;
    void *accumulator = acc();
    MPI_Status st;
    WkData_t *package;
    buff = malloc(MAX_PKG_SIZE);

    for (;;) {
        // Get input from manager
        MPI_OPEN_RECV(buff, &st, MAX_PKG_SIZE);
        package = unpack(buff);
        if (!package) {
            fprintf(stderr, "Error in synthesizer!\n");
            return;

        }
        memset(buff, 0, MAX_PKG_SIZE);

        // Break if msg is SN_GO
        if (package->msg == SN_GO) break;

        // Run synthesis on manager data
        synth(package->pkg, accumulator);
        if (package) free(package);

    }
    // Everything has exited, do output
    out(accumulator);

    // Cleanup and return
    if (buff) free(buff);
    return;

}

// Function deep copys a WkData_t struct into a plain 'ol buffer
static void *pack(WkData_t *package) {
    void *ret;
    char *pt;

    // Check size of data in package for overflow. If this happens, def. panic
    if (package->pkg_size > MAX_PKG_SIZE) {
        fprintf(stderr, "FATAL ERROR: Attempt to send %d ", (int)package->pkg_size);
        fprintf(stderr, " bytes, which is over the max allowed bytes.\n");
        return NULL;

    }
    // Create buffer
    ret = malloc(sizeof(int) + sizeof(size_t) + package->pkg_size);
    pt = ret;

    // Copy message
    memcpy(pt, (void *)&(package->msg), sizeof(int));
    //printf("Packed msg is: %d\n", *((int *)pt));
    pt += sizeof(int);

    // Copy pkg
    memcpy(pt, (void *)&(package->pkg_size), sizeof(size_t));
    //printf("Packed size is: %d\n", *((int *)pt));
    pt += sizeof(size_t);
    memcpy(pt, package->pkg, package->pkg_size);
    pt += package->pkg_size;

    // Return
    return ret;

}

// Function rebuilds struct from flat buffer
static WkData_t *unpack(void *buff) {
    WkData_t *ret;
    void *pkg_space;
    char *pt = (char *)buff;

    // Create struct
    ret = malloc(sizeof(WkData_t));

    // Copy message, size
    memcpy((void *)&(ret->msg), pt, sizeof(int));
    //printf("Unpacked msg is: %d.\n", ret->msg);
    pt += sizeof(int);
    memcpy((void *)&(ret->pkg_size), pt, sizeof(size_t));
    //printf("Unpacked size is: %d.\n", (int)ret->pkg_size);
    pt += sizeof(size_t);

    // Check pkg size. Same panic rule as above
    if (ret->pkg_size > MAX_PKG_SIZE) {
        fprintf(stderr, "FATAL ERROR: Attempt to recv %d ", (int)ret->pkg_size);
        fprintf(stderr, " bytes, which is over the max allowed bytes.\n");
        //fprintf(stderr, "MSG was: %d.\n", ret->msg);
        return NULL;

    }
    // Copy package
    pkg_space = malloc(ret->pkg_size);
    ret->pkg = pkg_space;
    memcpy(ret->pkg, pt, ret->pkg_size);

    // Cleanup and return
    return ret;

}
