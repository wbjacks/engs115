#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <mpi.h>
#include "wkman_optimized.h"
#include "../util/pqueue.h"

// Defines
#define PARTITION_MULTIPLIER 10
#define NUM_MAIN_PARTITIONS 10
#define OUTPUT_FILE "p4_output.csv"

// Statics
static void *calc(int rank, void *in, size_t *size);
static void part(void *args, void *qp);
static void r_part(double a, double b, double prec, void *qp);
static void synth(void *in, void *acc);
static void out(void *in);
static double f(double t);

// Structs
struct __partition {
    double a;
    double b;
    double prec;

};
typedef struct __partition Partition_t;

struct __part_input {
    double start;
    double end;
    double prec;
    int m;

};
typedef struct __part_input PartInput_t;

struct __calc_output {
    double val;
    double prec;
    int rank;

};
typedef struct __calc_output CalcOutput_t;

struct __accumulator {
    unsigned int *count;
    double value;
    double prec;
    int size;

};
typedef struct __accumulator Accumulator_t;

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
    parg->start = atof(argv[1]);
    parg->end = atof(argv[2]);
    parg->prec = atof(argv[3]);
    parg->m = 10;

    // Build accumulator
    acc = malloc(sizeof(Accumulator_t));
    memset(acc, 0, sizeof(Accumulator_t));
    acc->count = malloc(sizeof(unsigned int) * (size -2));
    memset(acc->count, 0, (sizeof(unsigned int) * (size -2)));
    acc->size = size;

    // call runWkMan with provided partitioner and calculator
    // Build partition arg
    runWkMan(argc, argv, parg, acc, calc, part, synth, out);

    // Exit
    return EXIT_SUCCESS;

}

// Taken and modified from lab2
static void part(void *args, void *qp) {
    int i;
    PartInput_t *input = (PartInput_t *)args;
    double step = (input->end - input->start) / input->m;

    // Queue loop
    for (i = 0; i < input->m; i++)
        r_part(input->start + i * step, input->start + (i+1) * step,
            input->prec, qp);

    // Cleanup and return
    return;

}

static void r_part(double a, double b, double prec, void *qp) {
    Partition_t *partition;

    // Check precision
    if (fabs(f(a)-f(b)) <= prec) {
        // Make partition
        partition = (Partition_t *)malloc(sizeof(Partition_t));
        partition->a = a;
        partition->b = b;
        partition->prec = prec;

        // Add to queue
        pqput(qp, (void *)partition, sizeof(Partition_t));

    }
    else {
        // Recurse
        r_part(a, (a + 0.5*(b - a)), prec, qp);
        r_part((a + 0.5*(b - a)), b, prec, qp);

    }
}

static void *calc(int rank, void *in, size_t *size) {
    double t;
    Partition_t *partition = (Partition_t *)in;
    CalcOutput_t *ret;
    ret = malloc(sizeof(CalcOutput_t));

    // Run calculation
    t = partition->b - partition->a;
    ret->val = 0.5 * t * (f(partition->b) + f(partition->a));

    *size = sizeof(CalcOutput_t);
    ret->rank = rank; // I could maybe do this with a tag?
    ret->prec = partition->prec;
    return (void *)ret;

}

static void synth(void *in, void *acc) {
    Accumulator_t *val = (Accumulator_t *)acc;
    CalcOutput_t *calc = (CalcOutput_t *)in;

    val->count[calc->rank - 1] += 1;
    val->value += calc->val;
    val->prec = calc->prec;
    
    return;

}

static void out(void *acc) {
    unsigned int max;
    int i, min;
    double average;
    FILE *fp;
    Accumulator_t *val = (Accumulator_t *)acc;

    // Set initials
    max = 0;
    min = -1;
    average = 0.0;

    // Calculate max, min, avg (linear search for small values of n)
    // Hit all but 0 (manager) and (size-1) (synth)
    for (i = 0; i < (val->size-2); i++) {
        // Check max
        if (max < *(val->count+i))
            max = *(val->count+i);

        // Check min
        if (min > *(val->count+i) || min == -1)
            min = *(val->count+i);

        // Calc running average
        average += ((double)*(val->count+i)) / ((double)(val->size-2));

    }
    printf("Max: %u, Min: %d, Average: %f.\n", max, min, average);
    printf("The value calculated is: %f.\n", val->value);
    fflush(stdout);

    // Write to file for CSV epicjoy
    fp = fopen(OUTPUT_FILE, "a");
    fprintf(fp, "%f, %d, %u, %f\n", val->prec, min, max, average);
    fclose(fp);
    
    return;

}


// The function I wish to calculate
static double f(double t) {
    return t * sin(t*t);

}
