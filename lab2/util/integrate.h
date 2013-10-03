// Includes
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <pthread.h>

// Structs
struct __thread_input {
    double start;
    double end;
    double p;
    int func_number;

};
typedef struct __thread_input ThreadInput_t;


struct __thread_output {
    double accumulator;
    int section_count;

};
typedef struct __thread_output ThreadOutput_t;

// Function prototypes

double integrate(double (*fp)(double t), double a,
    double b, double p, double f_sprev, int depth, int *final_depth);
ThreadOutput_t m_integrate(int func_number, double a, double b, double p, int m);
void *launcher(void *args);

