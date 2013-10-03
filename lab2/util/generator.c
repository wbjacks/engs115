// Includes
#include "./generator.h"

// Static function prototypes
static void *calculator(void *args);
static void *cruncher(void *args);

// Globals
GeneratorOutput_t output;
pthread_mutex_t q_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t a_lock = PTHREAD_MUTEX_INITIALIZER;
void *qp;

// Generator lauches m threads that compute the integral
// then, it begins to calculate a bunch of partitions to solve and enqueues them
// When done, it sets generator_complete to true

GeneratorOutput_t generator(double (*fp)(double t), double a, double b, double p, int m) {
    int i;
    void *status;
    // Create threads
    pthread_t threads[m+1]; 
    CalculatorInput_t c_in;

    // Launch all threads
    // First, launch calculator thread
    output.calc_complete = false;
    c_in.start = a;
    c_in.end = b;
    c_in.m = m;
    c_in.fp = fp;
    pthread_create(&threads[0], NULL, calculator, &c_in);

    // Then, launch and detach cruncher threads
    for (i = 1; i <= m; i++) {
        pthread_create(&threads[0], NULL, cruncher, NULL);

    }
    // Wait for calculator to return
    pthread_join(threads[0], &status);

    // Cleanup and return
    return output;

}

static void *calculator(void *args) {
    int i;
    CalculatorInput_t *input = (CalculatorInput_t *)args;
    QueueElement_t *elements[input->m * PARTITION_MULTIPLIER];

    // Create queue
    qp = qopen();
    
    // Queue loop
    for (i = 0; i < (input->m * PARTITION_MULTIPLIER); i++) {
        elements[i] = (QueueElement_t *)malloc(sizeof(QueueElement_t));
        elements[i]->a = input->start
            + i * ((input->end - input->start) / (input->m * PARTITION_MULTIPLIER));
        elements[i]->b = input->start
            + (i+1) * ((input->end - input->start) / (input->m * PARTITION_MULTIPLIER));
        elements[i]->fp = input->fp;

    }

    // Set calculator complete
    output.calc_complete = true;

    // Wait loop- replacing with a join in generator

    // Cleanup and return
    return NULL;
}

static void *cruncher(void *args) {
    double val;
    int f_depth;
    QueueElement_t *qe;
    // Wait loop
    while (!output.calc_complete && !isEmpty(qp)) {
        // CRITICAL SECTION
        // Grab element
        pthread_mutex_lock(&q_lock);
        qe = (QueueElement_t *)qget(qp);
        pthread_mutex_unlock(&q_lock);
        // END CRITICAL SECTION

        // Crunch division
        val = integrate(qe->fp, qe->a, qe->b, qe->p, 0, 0, &f_depth);
        
        // Grab lock
        pthread_mutex_lock(&a_lock);
        // CRITICAL SECTION
        // Add result to accumulator
        output.accumulator += val;
        pthread_mutex_unlock(&a_lock);
        // END CRITICAL SECTION
    }
    return NULL;
}
