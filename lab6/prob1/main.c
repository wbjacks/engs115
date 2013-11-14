#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "../util/wkman.h"
#include "../util/pqueue.h"

// Defines
#define PARTITION_MULTIPLIER 10
#define NUM_MAIN_PARTITIONS 10

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

};
typedef struct __partition Partition_t;

struct __part_input {
    double start;
    double end;
    double prec;
    int m;

};
typedef struct __part_input PartInput_t;

int main(int argc, char *argv[]) {
    void *acc;
    PartInput_t *parg;

    if (argc != 4) {
        fprintf(stderr, "Error: Bad arguments. Exiting...\n");
        return EXIT_FAILURE;

    }
    parg = malloc(sizeof(PartInput_t));
    parg->start = atof(argv[1]);
    parg->end = atof(argv[2]);
    parg->prec = atof(argv[3]);
    parg->m = 10;

    acc = malloc(sizeof(double));
    // call runWkMan with provided partitioner and calculator
    // Build partition args
    runWkMan(argc, argv, parg, acc, calc, part, synth, out);
    return EXIT_SUCCESS;

}

// Taken and modified from lab2
static void part(void *args, void *qp) {
    int i;
    PartInput_t *input = (PartInput_t *)args;
    double step = (input->end - input->start) / input->m;

    // Queue loop
    for (i = 0; i < input->m; i++)
        r_part(input->start + i * step, input->start + (i+1) * step, input->prec, qp);

    // Cleanup and return
    return;

}

static void r_part(double a, double b, double prec, void *qp) {
    Partition_t *partition;

    // Check precision
    if (fabs(a-b) <= prec) {
        // Make partition
        partition = (Partition_t *)malloc(sizeof(Partition_t));
        partition->a = a;
        partition->b = b;

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
    double *ret;
    double t;
    Partition_t *partition = (Partition_t *)in;
    ret = malloc(sizeof(double));

    // Run calculation
    t = partition->b - partition->a;
    *ret = 0.5 * t * (f(partition->b) + f(partition->a));

    *size = sizeof(double);
    return (void *)ret;

}

static void synth(void *in, void *acc) {
    *((double *)acc) += *((double *)in);
    return;

}

static void out(void *acc) {
    printf("The value of the integral is: %f.\n", *((double *)acc));
    return;

}


// The function I wish to calculate
static double f(double t) {
    return t*t*t;

}
