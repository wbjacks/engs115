#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <mpi.h>
#include "../util/wkman.h"
#include "../util/pqueue.h"
#include "../util/hostmap.h"
#include "../util/board.h"
#include "../util/spList.h"

// Defines
#define PARTITON_DEPTH 5
#define OUTPUT_FILE "./stat_output.csv"

// Structs
struct __partition {
    void *board;
    int row;
    int col;
    int n;

};
typedef struct __partition Partition_t;

struct __part_input {
    int n;
    void *qp;

};
typedef struct __part_input PartInput_t;

// Statics
static void calc(int rank, void *in, void *out);
static void r_calc(Partition_t *p, void *ret_list);
static void part(void *args, void *qp);
static void r_part(Partition_t *p, void *qp, int depth);
static void synth(void *in, void *acc);
static void out(void *in);
static void *packPartition(void *board, Partition_t *p, size_t *size);
static Partition_t *unpackPartition(void *buff);

// Partition output is Board_t

// Calc input is a partition

// Calc output is a Board_t

// Accumulator is a hostmap

int main(int argc, char *argv[]) {
    int size, rank;
    void *acc;
    PartInput_t *parg;

    if (argc != 2) {
        fprintf(stderr, "Error: Bad arguments. Exiting...\n");
        return EXIT_FAILURE;

    }
    // Begin MPI
    MPI_Init(&argc, &argv);

    // Get rank / size
    MPI_RANK_SIZE(&rank, &size);

    // Build partition input
    parg = malloc(sizeof(PartInput_t));
    parg->n = atoi(argv[1]);
    parg->qp = pqopen();

    // Build accumulator
    acc = createMap();
    addCurrentHost(acc);

    // call runWkMan with provided partitioner and calculator
    // Build partition arg
    runWkMan(argc, argv, parg, acc, calc, part, synth, out);

    // Exit
    return EXIT_SUCCESS;

}

static void part(void *args, void *qp) {
    void *initial_board;
    PartInput_t *input = (PartInput_t *)args;
    Partition_t *pp;

    initial_board = initialize(input->n);
    pp = (Partition_t *)malloc(sizeof(Partition_t));
    pp->board = initial_board;
    pp->row = 0;
    pp->col = 0;
    pp->n = input->n;
    r_part(pp, qp, 0);

}

static void r_part(Partition_t *p, void *qp, int depth) {
    void *buff, *new_board;
    size_t packed_size;
    Partition_t *sub;

    // Base case 1: solved
    if (numSetQueens(p->board) == p->n) {
        // Return board to queue
        buff = packPartition(p->board, p, &packed_size);
        pqput(qp, buff, packed_size);

    }
    // Base case 2: depth satisfied
    else if (depth == PARTITON_DEPTH) {
        buff = packPartition(p->board, p, &packed_size);
        pqput(qp, buff, packed_size);

    }
    // Recursive case
    else if (onBoard(p->board, p->row, p->col)) {
        if (isLegalMove(p->board, p->row, p->col)) {
            // Launch new partition with this move placed
            new_board = boardCpy(p->board);
            setQueen(new_board, p->row, p->col);

            // SUBPROBLEM
            // Recurse on subproblem
            // Create new partition
            sub = (Partition_t *)malloc(sizeof(Partition_t));
            sub->row = p->row+1;
            sub->col = 0;
            sub->n = p->n;
            sub->board = new_board;
            r_part(sub, qp, depth+1);

        }
        // Recurse original problem
        p->col = p->col+1;
        r_part(p, qp, depth+1);

    }
    // Terminal?
    else {
        cleanUp(p->board);
        //SAFE_FREE(p);

    }
}

static void *packPartition(void *board, Partition_t *p, size_t *size) {
    char *pt;
    void *ret;
    size_t b_size;

    ret = packBoard(board, &b_size);
    ret = realloc(ret, b_size + sizeof(Partition_t));
    pt = (char *)ret;
    pt += b_size;

    // Pack things
    memcpy(pt, &(p->row), sizeof(int));
    pt += sizeof(int);
    memcpy(pt, &(p->col), sizeof(int));
    pt += sizeof(int);
    memcpy(pt, &(p->n), sizeof(int));
    pt += sizeof(int);

    // Return
    *size = pt - (char *)ret;
    return ret;

}

static Partition_t *unpackPartition(void *buff) {
    char *pt;
    size_t size;
    Partition_t *p = (Partition_t *)malloc(sizeof(Partition_t));

    pt = (char *)buff;

    // Unpack board
    p->board = unpackBoard(pt, &size);
    pt += size;

    // Unpack partition
    memcpy(&(p->row), pt, sizeof(int));
    pt += sizeof(int);
    memcpy(&(p->col), pt, sizeof(int));
    pt += sizeof(int);
    memcpy(&(p->n), pt, sizeof(int));
    pt += sizeof(int);

    return p;

}

static void calc(int rank, void *in, void *out) {
    Partition_t *p;

    p = unpackPartition(in);
    r_calc(p, out);

}

static void r_calc(Partition_t *p, void *ret_list) {
    void *buff, *new_board;
    size_t packed_size;
    Partition_t *sub;

    // Base case 1: solved
    if (numSetQueens(p->board) == p->n) {
        // Add solution to ret_list
        buff = packBoard(p->board, &packed_size);
        spLAddProb(ret_list, buff, packed_size, TRUE);
        //cleanUp(p->board);
        //SAFE_FREE(p);

    }
    // Recursive case
    else if (onBoard(p->board, p->row, p->col)) {
        if (isLegalMove(p->board, p->row, p->col)) {
            // Launch new partition with this move placed
            new_board = boardCpy(p->board);
            setQueen(new_board, p->row, p->col);
            // Create new partition
            sub = (Partition_t *)malloc(sizeof(Partition_t));
            sub->row = p->row+1;
            sub->col = 0;
            sub->n = p->n;
            sub->board = new_board;

            // Recurse
            r_calc(sub, ret_list);

        }
        p->col = p->col+1;
        r_calc(p, ret_list);

    }
    // Terminal case
    else {
        //cleanUp(p->board);
        //SAFE_FREE(p);

    }
}

static void synth(void *b, void *hostmap) {
    void *board;
    size_t size;

    /* The below will crash my computer */
    // Print board (optional?)
    board = unpackBoard(b, &size);
    printBoard(board);
    fflush(stdout);

    // Increment problem for computer (check host...)
    addValToHost(hostmap, 1);
    return;

}

static void out(void *hostmap) {
    FILE *fp;
    int *vals, i;
    size_t size;
    vals = getAllValues(hostmap, &size);

    fp = fopen(OUTPUT_FILE, "a");
    for (i = 0; i < size; i++)  {
        fprintf(fp, "%d, %d\n", i, vals[i]);

    }
    fclose(fp);
    return;

}
