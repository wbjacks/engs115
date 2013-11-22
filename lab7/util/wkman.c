#include "wkman.h"

// Global
int partition_done = FALSE;

// Allows me to pass in 2-for-1, which lets me partition in a thread
struct __part_bundle {
    void *args;
    void *qp;
    void (*partitioner)(void *, void *);

};
typedef struct __part_bundle PartBundle_t;

// Statics
static void worker(int size, int rank, void *(*calc)(int rank, void *, size_t *size));
static void manager(int size, void *args, void (*partitioner)(void *args, void *qp));
static void *part_wrapper(void *pb); // for multi-threading
static void synthesizer(void *acc, void (*synth)(void *, void *), void (*out)(void *));

int runWkMan(int argc,
             char *argv[],
             void *pargs,
             void *acc,
             void *(*calc)(int rank, void *, size_t *size),
             void (*part)(void *, void *),
             void (*synth)(void *, void *),
             void (*out)(void *))
{
    int rank, size;
    int initialized;

    // Check if initialized
    MPI_Initialized(&initialized);
    if (!initialized) MPI_Init(&argc, &argv);

    MPI_RANK_SIZE(&rank, &size);

    // Launch
    if (rank == 0)
        manager(size, pargs, part);

    else if(rank == (size -1))
        synthesizer(acc, synth, out);

    else 
        worker(size, rank, calc);
 
    // Exit
    MPI_Finalize();
    return 0;

}

static void worker(int size, int rank, void *(*calc)(int rank, void *, size_t *size)) {
    int kill = FALSE;
    void *buff;
    void *r_buff;
    void *ret_pkg;
    size_t ret_pkg_size;
    size_t packed_size;
    MPI_Status st;
    WkData_t *data;
    WkData_t ret_data;

    r_buff = malloc(MAX_PKG_SIZE);

    // Send initial ready message
    ret_data.msg = WK_READY;
    ret_data.pkg_size = sizeof(int);
    ret_data.pkg = malloc(sizeof(int));
    memcpy(ret_data.pkg, &rank, sizeof(int));
    buff = pack(&ret_data, &packed_size);
    MPI_OPEN_SEND(buff, 0, packed_size);
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

        // Recieved message
        if (data->msg != NO_MSG) {
            switch (data->msg) {
                case WK_KILL: kill = TRUE; break;
                case WK_WAIT:
                    // Sleep, then send ready
                    sleep(WORKER_WAIT_TIME);
                    // Consider making the below a macro
                    ret_data.msg = WK_AWAKE;
                    ret_data.pkg_size = sizeof(int);
                    ret_data.pkg = malloc(sizeof(int));
                    memcpy(ret_data.pkg, &rank, sizeof(int));
                    buff = pack(&ret_data, &packed_size);
                    MPI_OPEN_SEND(buff, 0, packed_size);
            if (buff) free(buff);
                    break;
                default:
                    fprintf(stderr, "Warning: Bad message received at worker %d.\n", rank);
                    break;

            }
        }
        // Recieved partition packet
        else {
            // If message is empty, throw pkg at the supplied task
            printf("Calculating...\n");
            ret_pkg = calc(rank, data->pkg, &ret_pkg_size);

            /* NEW PROBLEM: calc can either return a sub-problem (branching) */
            /* or the original problem */

            // Pack data, send to sync 
            ret_data.msg = NO_MSG;
            ret_data.pkg_size = ret_pkg_size;
            ret_data.pkg = ret_pkg;
            buff = pack(&ret_data, &packed_size);

            // Send data back
            MPI_OPEN_SEND(buff, (size-1), packed_size);
            if (buff) free(buff);

            // Send WK_READY to manager
            ret_data.msg = WK_READY;
            ret_data.pkg_size = sizeof(int);
            ret_data.pkg = malloc(sizeof(int));
            memcpy(ret_data.pkg, &rank, sizeof(int));
            buff = pack(&ret_data, &packed_size);
            MPI_OPEN_SEND(buff, 0, packed_size);
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
    buff = pack(&ret_data, &packed_size);
    MPI_OPEN_SEND(buff, 0, packed_size);
    return;

}

static void manager(int size, void *args, void (*partitioner)(void *args, void *qp)) {
    int done_workers = 0, waiting_workers = 0;
    void *buff, *r_buff;
    void *qval;
    void *qp;
    size_t qval_size, packed_size;
    MPI_Status st;
    WkData_t *data;
    WkData_t ret_data;
    PartBundle_t *pb;
    pthread_t part_thread;

    r_buff = malloc(MAX_PKG_SIZE);

    // Open shit
    qp = pqopen();
    pb = malloc(sizeof(PartBundle_t));
    pb->qp = qp;
    pb->args = args;
    pb->partitioner = partitioner;

    // Run partitioner-> REACH do this in a thread
    pthread_create(&part_thread, NULL, part_wrapper, pb);
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
            if (data->msg == WK_AWAKE) {
                waiting_workers--;
                printf("Waiting: %d.\n", waiting_workers);

            }
            // Grab value from queue
            qval = pqget(qp, &qval_size);
            if (!qval && partition_done && waiting_workers == (size-2)) {
                // If queue value is NULL, send WK_KILL
                // Make sure pthread isn't still going
                //pthread_join(part_thread, NULL);

                ret_data.msg = WK_KILL;
                ret_data.pkg_size = 0;
                ret_data.pkg = NULL;

            }
            else if (qval != NULL) {
                // Qvalue received, send it along
                //printf("Size from queue: %d\n", (int)qval_size);
                printf("pkg\n");
                ret_data.msg = NO_MSG;
                ret_data.pkg_size = qval_size;
                ret_data.pkg = qval;

            }
            else {
                // Else qvalue not obtained, tell worker to wait
                ret_data.msg = WK_WAIT;
                ret_data.pkg_size = 0;
                ret_data.pkg = NULL;
                waiting_workers++;

            }
            // Send
            buff = pack(&ret_data, &packed_size);
            MPI_OPEN_SEND(buff, *((int *)data->pkg), packed_size);
            if (buff) free(buff);

        }
        if (data) free(data);

    }

    // Send sync_ok
    ret_data.msg = SN_GO;
    ret_data.pkg_size = 0;
    ret_data.pkg = NULL;
    buff = pack(&ret_data, &packed_size);
    MPI_OPEN_SEND(buff, (size-1), packed_size);
    if (buff) free(buff);

    // Cleanup and return
    pqclose(qp);
    return;

}

static void *part_wrapper(void *p) {
    PartBundle_t *pb = (PartBundle_t *)p;

    pb->partitioner(pb->args, pb->qp);
    partition_done = TRUE;
    return NULL;

}

static void synthesizer(void *acc, void (*synth)(void *, void *), void (*out)(void *)) {
    // Manages program output
    void *buff;
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
        synth(package->pkg, acc);
        if (package) free(package);

    }
    // Everything has exited, do output
    out(acc);

    // Cleanup and return
    if (buff) free(buff);
    return;

}

// Function deep copys a WkData_t struct into a plain 'ol buffer
void *pack(WkData_t *package, size_t *size) {
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

    // Calc size
    *size = (size_t)(pt - (char *)ret);

    // Return
    return ret;

}

// Function rebuilds struct from flat buffer
WkData_t *unpack(void *buff) {
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
