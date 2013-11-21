#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <mpi.h>
#include "wkman_optimized.h"
#include "../util/pqueue.h"

// Defines
#define PARTITION_MULTIPLIER 10
#define NUM_MAIN_PARTITIONS 10
#define OUTPUT_FILE "./stat_output.csv"

// Statics
static void *calc(int rank, void *in, size_t *size);
static void part(void *args, void *qp);
static void r_part(double a, double b, double prec, void *qp);
static void synth(void *in, void *acc);
static void out(void *in);
static double f(double t);

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

};
typedef struct __part_input PartInput_t;

// Partition output is Board_t

// Calc input is a partition

// Calc output is a Board_t

// Accumulator is a hostmap

int main(int argc, char *argv[]) {
    PartInput_t *parg;
    Accumulator_t *acc;
    int size, rank;

    if (argc != 4) {
        fprintf(stderr, "Error: Bad arguments. Exiting...\n");
        return EXIT_FAILURE;

    }
    // Begin MPI
    MPI_Init(&argc, &argv);

    // Get rank / size
    MPI_RANK_SIZE(&rank, &size);

    // Build partition input
    parg = malloc(sizeof(PartInput_t));

    // Build accumulator
    acc = malloc(sizeof(Accumulator_t));

    // call runWkMan with provided partitioner and calculator
    // Build partition arg
    runWkMan(argc, argv, parg, acc, calc, part, synth, out);

    // Exit
    return EXIT_SUCCESS;

}

static void part(void *args, void *qp) {
    PartInput_t *input = (PartInput_t *)args;
    Partition_t *p;
    void *b;

    for (i = 0; i < input->n; i++) {
        b = initialize(input->n);
        p = (Partition_t *)malloc(sizeof(Partition_t));
        setQueen(b, 0, i);
        p->board = b;
        p->row = 1;
        p->col = 0;
        p->n = input->n;
        // PACK PARTITION INTO BUFFER
        pqput(p, sizeof(Partition_t)); // Error: this needs to be flat. Make pack

    }
    // Cleanup and return
    return;

}

static void *calc(int rank, void *in, size_t *size) {
    Partition_t *p = (Partition_t *)in;
    void *ret = NULL;

    // Base case 1: solved
    if (numSetQueens(p->board) == p->n) {
        // Send board to synth
        ret = malloc(sizeof(Board_t));
        ret = p->board;

    }
    // Recursive case
    else if (onBoard(p->board, p->row, p->col)) {
        if (isLegalMove(p->board, p->row, p->col)) {
            // Launch new partition with this move placed
            new_board = boardCpy(p->board);
            setQueen(new_board, p->row, p->col);
            // Add to queue- send WK_SUBPROB
            MPI_OPEN_SEND(/*subprob*/);

        }
        // Return board to queue at new r,c value
        solve(board, n, row, col+1);
        MPI_OPEN_SEND(/*subprob*/);

    }
    return ret;

}

static void synth(void *board, void *hostmap) {
    // Print board (optional?)
    if (board)
        printBoard(board);

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
