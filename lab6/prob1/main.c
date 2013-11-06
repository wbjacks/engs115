#include <stdlib.h>
#include <stdio.h>

// Defines
#define PARTITION_MULTIPLIER 10

// Statics
static void *calc();
static void part();
static void synth(void *in, void *acc);
static void *acc(void);
static void out(void *in);
static double f(double t);

// Structs
struct __partition {
    double a;
    double b;
    double p;

};
typedef struct __partition Partition_t;

struct __part_input {
    void *qp;
    int m;

};
typedef struct __part_input PartInput_t;

int main(int argc, char *argv[]) {
    // call runWkMan with provided partitioner and calculator
    runWkMan(argc, argv, acc, calc, part, synth, out);

}

// Taken and modified from lab2
static void part(void *args) {
    int i;
    PartInput_t *input = (PartInput_t *)args;
    Partition_t *partition;
    double step = (input->end - input->start) / (input->m * PARTITION_MULTIPLIER);

    // Queue loop
    for (i = 0; i < (input->m * PARTITION_MULTIPLIER); i++) {
        partition = (Partition_t *)malloc(sizeof(Partition_t));
        partition->a = input->start + i * step;
        partition->b = input->start + (i+1) * step;
        partition->p = input->p;

        qput(input->queue->qp, (void *)partition, sizeof(Partition_t));

    }
    // Cleanup and return
    return;

}

static void *calc(void *in) {
    double *ret;
    double t;
    Partition_t partition = (Partition_t *)in;
    ret = malloc(sizeof(double));

    // Run calculation
    t = partition->b - partition->a;
    *ret = 0.5 * t * (f(partition->b) + f(partition->a));

    return (void *)ret;

}

static void synth(void *in, void *acc) {
    *((double *)acc) += *((double *)in);
    return;

}

static void *acc(void) {
    return malloc(sizeof(double));

}

static void out(void *acc) {
    printf("The value of the integral is: %f", *((double *)acc));
    return;

}


// The function I wish to calculate
static double f(double t) {
    return t*t*t;

}