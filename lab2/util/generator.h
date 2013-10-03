// Includes
#include <pthread.h>
#include <stdlib.h>
#include <stdbool.h>
#include "queue.h"
#include "integrate.h"

// Defines
#define PARTITION_MULTIPLIER 15
#define CALCULATOR_WAIT_TIME 0.001

// Structs
struct __generator_output {
    double accumulator;
    bool calc_complete;

};
typedef struct __generator_output GeneratorOutput_t;

struct __calculator_input {
    double start;
    double end;
    int m;
    double (*fp)(double time);

};
typedef struct __calculator_input CalculatorInput_t;

struct __queue_element {
    double a;
    double b;
    double p;
    double (*fp)(double time);

};
typedef struct __queue_element QueueElement_t;

// Function prototypes
GeneratorOutput_t generator(double (*fp)(double t), double a, double b, double p, int m);
