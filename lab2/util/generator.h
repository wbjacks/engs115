// Includes
#include <pthread.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include "queue.h"
#include "integrate.h"

// Defines
#define PARTITION_MULTIPLIER 15
#define CRUNCHER_WAIT_TIME 0.001

// Structs
// For queue
struct __thread_queue {
    void *qp;
    int num_elements;

};
typedef struct __thread_queue ThreadQueue_t;

struct __queue_element {
    double a;
    double b;
    double p;
    double (*fp)(double time);

};
typedef struct __queue_element QueueElement_t;

// Global output
struct __generator_output {
    double accumulator;
    unsigned long depth;
    bool calc_complete;

};
typedef struct __generator_output GeneratorOutput_t;

// Thread input
struct __calculator_input {
    double start;
    double end;
    int m;
    double (*fp)(double time);
    double p;
    ThreadQueue_t *queue;

};
typedef struct __calculator_input CalculatorInput_t;

struct __cruncher_input {
    ThreadQueue_t *queue;

};
typedef struct __cruncher_input CruncherInput_t;


// Function prototypes
GeneratorOutput_t generator(double (*fp)(double t), double a, double b, double p, int m);
