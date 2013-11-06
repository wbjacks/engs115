#include "wkman.h"

// Statics
static void worker(int size, int rank);
static void manager(int size);
static void *pack(struct __wk_data *package);
static struct __wk_data *unpack(void *package, size_t size);

// Internal Structures
struct __wk_data {
    int msg;
    int pkg_size;
    void *pkg;

};
typedef struct __wk_data WkData_t;

int runWkMan(int argc, char *argv[], void *(*fp)(void *)) {
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
        worker(size, rank, fp);

    }
    // Exit
    MPI_Finalize();
    return 0;

}

static void worker(int size, int rank, void *(*task)(void *)) {
    int msg;  
    int kill = FALSE;
    int ret_pkg_size;
    void *buff;
    void *ret_pkg;
    MPI_Status st;
    WkData_t *data;
    WkData_t ret_data;

    // Send initial ready message
    ret_data.msg = WK_READY;
    buff = pack(ret_data);
    MPI_OPEN_SEND(buff, 0, MAX_PKG_SIZE);

    // Loop until kill is set
    while (!kill) {
        // Wait for message from manager, unpack, clear buffer
        buff = malloc(MAX_PKG_SIZE);
        MPI_OPEN_RECV(buff, &st, MAX_PKG_SIZE);
        data = unpack(buff);
        free(buff);

        // Recieved message... open
        if (data->msg != NO_MSG) {
            switch (data->msg) {
                case WK_KILL: kill = TRUE; break;
                default:
                    fprintf(stderr, "Warning: Bad message recieved at worker %d.\n", rank);
                    break;

            }
        }
        else {
            // If message is empty, throw pkg at the supplied task
            ret_pkg = task(data->pkg, &ret_pkg_size);

            // Pack data 
            ret_data.msg = WK_READY;
            ret_data.pkg_size = ret_pkg_size;
            ret_data.pkg = ret_pkg;
            buff = pack(ret_data);

            // Send data back
            MPI_OPEN_SEND(buff, 0, MAX_PKG_SIZE);

            // Cleanup
            memset(ret_pkg, 0, ret_pkg_size);

        }
    }
    // Kill has been set... clean up and die in peace
    ret_data.msg = WK_DONE;
    buff = pack(ret_data);
    MPI_OPEN_SEND(buff, 0, MAX_PKG_SIZE);
    return;

}

static void manager(int size, void (*partitioner)(void *qp)) {
    int i;
    int done_workers;
    int qval_size;
    void *buff;
    void *qp;
    void *qval;
    MPI_Status st;
    WkData_t *data;
    WkData_t ret_data;

    // Open shit
    qp = pqopen();

    // Run partitioner-> REACH do this in a thread
    partitioner(qp);

    // When returned, everything will be fully partitioned

    // Loop until workers done
    while (done_workers < (size-1)) {
        // Receive message, check
        buff = malloc(MAX_PKG_SIZE);
        MPI_OPEN_RECV(buff, &st, MAX_PKG_SIZE);
        data = unpack(buff);
        free(buff); // error, yo, use realloc
        if (data->msg == WK_DONE) 
            done_workers++;

        else {
            // Grab value from queue
            qval = qget(qp, &qval_size);
            if (!qval) {
                // If queue value is NULL, send WK_KILL
                ret_data.msg = WK_KILL;
                buff = pack(ret_data);

            }
            else {
                // Else, send partition
                ret_data.msg = NO_MSG;
                ret_data.pkg_size = qval_size;
                ret_data.pkg = qval;

            }
            MPI_OPEN_SEND(buff, (*(int *)data->pkg), MAX_PKG_SIZE);

        }
        free(data);

    }
    // Cleanup and return
    qclose(qp);
    return;

}

// Function deep copys a WkData_t struct into a plain 'ol buffer
static void *pack(struct __wk_data *package) {
    void *ret;

    // Check size of data in package for overflow. If this happens, def. panic
    if (package->pkg_size > MAX_PKG_SIZE) {
        fprintf(stderr, "FATAL ERROR: Attempt to send too much data.\n");
        return NULL;

    }

    // Create buffer
    ret = malloc(2*sizeof(int) + sizeof(package->pkg_size));

    // Copy message
    ret = malloc(1 * sizeof(int));
    memcpy(ret, (void *)&(package->msg), sizeof(int));

    // Copy pkg
    memcpy(ret, (void *)&(package->pkg_size), sizeof(int));
    memcpy(ret, package->pkg, package->pkg_size);

    // Return
    return ret;

}

// Function rebuids struct from flat buffer
static struct __wk_data *unpack(void *buff) {
    WkData_t *ret;
    void *pkg_space;
    void *pt = buff;

    // Create struct
    ret = malloc(sizeof(WkData_t));

    // Copy message, size
    memcpy(ret->msg, pt, sizeof(int));
    pt += sizeof(int);
    memcpy(ret->pkg_size, pt, sizeof(int));
    pt += sizeof(int);

    // Check pkg size. Same panic rule as above
    if (ret->pkg_size > MAX_PKG_SIZE) {
        fprintf(stderr, "FATAL ERROR: Attempt to recv too much data.\n");
        return NULL;

    }
    // Copy package
    pkg_space = malloc(ret->pkg_size);
    ret->pkg = pkg_space;
    memcpy(ret->pkg, pt, ret->pkg_size);

    // Cleanup and return
    return ret;

}
