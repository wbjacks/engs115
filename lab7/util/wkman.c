#include "wkman.h"
#include "board.h"

// Global
/*
int all_waiting = FALSE;
int is_waiting = FALSE;
*/
int partition_done = FALSE;
int kill = FALSE;

// Allows me to pass in 2-for-1, which lets me partition in a thread
struct __part_bundle {
    void *args;
    void *qp;
    void (*partitioner)(void *, void *);

};
typedef struct __part_bundle PartBundle_t;

struct __tkn_args {
    int rank;
    int size;

};
typedef struct __tkn_args TknArgs_t;

// Statics
static void worker(int size, int rank, void (*calc)(int, void *, void *));
static void manager(int size, void *args, void (*partitioner)(void *, void *));
static void *part_wrapper(void *pb); // for multi-threading
static void synthesizer(void *acc, void (*synth)(void *, void *), void (*out)(void *));
//static void *workerToken(void *args);
//static void *managerToken(void *args);

int runWkMan(int argc,
             char *argv[],
             void *pargs,
             void *acc,
             void (*calc)(int, void *, void *),
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

static void worker(int size, int rank, void (*calc)(int rank, void *, void *)) {
    void *buff;
    void *r_buff;
    void *ret_pkg;
    void *subprob;
    size_t ret_pkg_size;
    size_t packed_size;
    MPI_Status st;
    WkData_t *data;
    WkData_t ret_data;
    //pthread_t token_thread;
    //TknArgs_t targs;

    r_buff = malloc(MAX_PKG_SIZE);

    // Send initial ready message
    ret_data.msg = WK_READY;
    ret_data.pkg_size = sizeof(int);
    ret_data.pkg = malloc(sizeof(int));
    memcpy(ret_data.pkg, &rank, sizeof(int));
    buff = pack(&ret_data, &packed_size);
    MPI_OPEN_SEND(buff, 0, packed_size);
    SAFE_FREE(buff);

    // Start token passer
    /*
    targs.rank = rank;
    targs.size = size;
    pthread_create(&token_thread, NULL, workerToken, &targs);
    */

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
                    //is_waiting = TRUE; break;
                    // Sleep, then send ready
                    // (sorry professor can't get token passing to work)
                    sleep(WORKER_WAIT_TIME);
                    // Consider making the below a macro
                    ret_data.msg = WK_READY;
                    ret_data.pkg_size = sizeof(int);
                    ret_data.pkg = malloc(sizeof(int));
                    memcpy(ret_data.pkg, &rank, sizeof(int));
                    buff = pack(&ret_data, &packed_size);
                    MPI_OPEN_SEND(buff, 0, packed_size);
                    SAFE_FREE(buff);
                    break;

                default:
                    fprintf(stderr,
                        "Warning: Bad message %d received at worker %d.\n", data->msg, rank);
                    break;

            }
        }
        // Recieved partition packet
        else {
            // If message is empty, throw pkg at the supplied task
            /*
            all_waiting = FALSE;
            is_waiting = FALSE;
            */
            subprob = spLOpen();
            calc(rank, data->pkg, subprob);

            /* NEW PROBLEM: calc returns a list of problems */
            while ((ret_pkg = spLGetProb(subprob, &ret_pkg_size)) != NULL) {
                // Pack data, send to sync 

                // Test test
                //printf("Return from calc.");
                //printf("\n\nChecking return of calc...\n");
                //printBoard(unpackBoard(ret_pkg, &packed_size));

                ret_data.msg = NO_MSG;
                ret_data.pkg_size = ret_pkg_size;
                ret_data.pkg = ret_pkg;
                buff = pack(&ret_data, &packed_size);

                // Send data back
                MPI_OPEN_SEND(buff, (size-1), packed_size);
                SAFE_FREE(buff);

            }
            spLClose(subprob);

            // Send WK_READY to manager
            ret_data.msg = WK_READY;
            ret_data.pkg_size = sizeof(int);
            ret_data.pkg = malloc(sizeof(int));
            memcpy(ret_data.pkg, &rank, sizeof(int));
            buff = pack(&ret_data, &packed_size);
            MPI_OPEN_SEND(buff, 0, packed_size);
            SAFE_FREE(buff);

            // Cleanup
            memset(ret_pkg, 0, ret_pkg_size);

        }
        SAFE_FREE(data);
    
    }
    // Kill has been set... clean up and die in peace
    //pthread_join(token_thread, NULL);

    ret_data.msg = WK_DONE;
    ret_data.pkg_size = 0;
    ret_data.pkg = NULL;
    buff = pack(&ret_data, &packed_size);
    MPI_OPEN_SEND(buff, 0, packed_size);
    return;

}

/*
static void *workerToken(void *in) {
    int src, next;
    void *buff;
    size_t packed_size;
    MPI_Status st;
    WkData_t ret_data;
    TknArgs_t *args = (TknArgs_t *)in;

    // Find next
    next = (args->rank + 1) % args->size;
    if (next == 0) next = 1;
    if (next == (args->size-1)) next = 1;

    for(;;) {
        // Wait for tagged message
        MPI_TKN_RECV(&src, &st, sizeof(int));

        if (all_waiting) {
            if (src != next)
                MPI_TKN_SEND((void *)&src, next, sizeof(int));
            return NULL;

        }
        // Check if waiting
        else if (is_waiting) {
            // Check src of token
            if (src == args->rank) {
                // set all_waiting-> send msg to manager
                all_waiting = TRUE;
                MPI_TKN_SEND((void *)&src, 0, sizeof(int)); // poke manager

            }
            // Else, check if src = -1
            if (src == -1) {
                src = args->rank;

            }
            // Try to signal manager
            ret_data.msg = WK_READY;
            ret_data.pkg_size = sizeof(int);
            ret_data.pkg = malloc(sizeof(int));
            memcpy(ret_data.pkg, &args->rank, sizeof(int));
            buff = pack(&ret_data, &packed_size);
            MPI_OPEN_SEND(buff, 0, packed_size);
            SAFE_FREE(buff);

        }
        // Else, set src = -1
        else {
            src = -1;

        }
        // Pass to next
        MPI_TKN_SEND((void *)&src, next, sizeof(int));

    }
}
*/

static void manager(int size, void *args, void (*partitioner)(void *args, void *qp)) {
    int done_workers = 0;
    //int src;
    void *buff, *r_buff;
    void *qval;
    void *qp;
    size_t qval_size, packed_size;
    MPI_Status st;
    WkData_t *data;
    WkData_t ret_data;
    PartBundle_t *pb;
    pthread_t part_thread;//, token_thread;

    r_buff = malloc(MAX_PKG_SIZE);

    // Open shit
    qp = pqopen();
    pb = malloc(sizeof(PartBundle_t));
    pb->qp = qp;
    pb->args = args;
    pb->partitioner = partitioner;

    // Run token pass
    //pthread_create(&token_thread, NULL, managerToken, NULL);

    // Run partitioner-> REACH do this in a thread
    pthread_create(&part_thread, NULL, part_wrapper, pb);

    // Partitioner done, start token passing
    /*
    src = -1;
    MPI_TKN_SEND((void *)&src, 1, sizeof(int));
    */

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
        if (data->msg == WK_DONE) { 
            done_workers++;
            //printf("Workers done: %d.\n", done_workers);

        }
        else {
            // Grab value from queue
            qval = pqget(qp, &qval_size);
            if (!qval && partition_done) { //&& all_waiting) {
                // If queue value is NULL, send WK_KILL
                // Make sure pthread isn't still going
                //pthread_join(part_thread, NULL);

                ret_data.msg = WK_KILL;
                ret_data.pkg_size = 0;
                ret_data.pkg = NULL;
                //printf("Sending kill ");

            }
            else if (qval != NULL) {
                // Qvalue received, send it along
                //printf("Size from queue: %d\n", (int)qval_size);
                ret_data.msg = NO_MSG;
                ret_data.pkg_size = qval_size;
                ret_data.pkg = qval;

            }
            else {
                // Else qvalue not obtained, tell worker to wait
                ret_data.msg = WK_WAIT;
                ret_data.pkg_size = 0;
                ret_data.pkg = NULL;

            }
            // Send
            buff = pack(&ret_data, &packed_size);
            MPI_OPEN_SEND(buff, *((int *)data->pkg), packed_size);
            //printf("to %d.\n", *((int *)data->pkg));
            SAFE_FREE(buff);

        }
        SAFE_FREE(data);

    }

    // Send sync_ok
    ret_data.msg = SN_GO;
    ret_data.pkg_size = 0;
    ret_data.pkg = NULL;
    buff = pack(&ret_data, &packed_size);
    MPI_OPEN_SEND(buff, (size-1), packed_size);
    SAFE_FREE(buff);

    // Cleanup and return
    pqclose(qp);
    return;

}

/*
static void *managerToken(void *args) {
    int src;
    MPI_Status st;
    // Wait for message
    MPI_TKN_RECV(&src, &st, sizeof(int));

    // Set all_waiting
    all_waiting = TRUE;

    // Terminate
    return NULL;

}
*/

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
        free(package);

    }
    // Everything has exited, do output
    out(acc);

    // Cleanup and return
    SAFE_FREE(buff);
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
