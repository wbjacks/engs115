// Includes
#include "./generator.h"

// Static function prototypes
static void *calculator(void *args);
static void *cruncher(void *args);

// Globals
GeneratorOutput_t output;
pthread_mutex_t q_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t a_lock = PTHREAD_MUTEX_INITIALIZER;

// Generator lauches m threads that compute the integral
// then, it begins to calculate a bunch of partitions to solve and enqueues them
// When done, it sets generator_complete to true

GeneratorOutput_t generator(double (*fp)(double t), double a, double b, double p, int m) {
    int i;
    void *status;
    ThreadQueue_t *tqp;
    tqp = (ThreadQueue_t *)malloc(sizeof(ThreadQueue_t));
    tqp->qp = qopen();
    // Create threads
    pthread_t threads[m+1]; 
    CalculatorInput_t ca_in;
    CruncherInput_t cr_in[m];

    // Launch all threads
    // First, launch calculator thread
    output.calc_complete = false;
    output.depth = 0;
    output.accumulator = 0.0;
    ca_in.start = a;
    ca_in.end = b;
    ca_in.m = m;
    ca_in.p = p;
    ca_in.fp = fp;
    ca_in.queue = tqp;
    pthread_create(&threads[0], NULL, calculator, &ca_in);

    // Then, launch and detach cruncher threads
    for (i = 1; i <= m; i++) {
        cr_in[i-1].queue = tqp;
        pthread_create(&threads[i], NULL, cruncher, &cr_in[i-1]);

    }
    // Wait for all to return
    for (i = 0; i <= m; i++)
        pthread_join(threads[i], &status);

    // Cleanup and return
    return output;

}

static void *calculator(void *args) {
    int i;
    CalculatorInput_t *input = (CalculatorInput_t *)args;
    QueueElement_t *elements[input->m * PARTITION_MULTIPLIER];

    // Create queue
    //qp = qopen();
    
    // Queue loop
    for (i = 0; i < (input->m * PARTITION_MULTIPLIER); i++) {
        elements[i] = (QueueElement_t *)malloc(sizeof(QueueElement_t));
        elements[i]->a = input->start
            + i * ((input->end - input->start) / (input->m * PARTITION_MULTIPLIER));
        elements[i]->b = input->start
            + (i+1) * ((input->end - input->start) / (input->m * PARTITION_MULTIPLIER));
        elements[i]->fp = input->fp;
        elements[i]->p = input->p;

        // CRITICAL SECTION
        pthread_mutex_lock(&q_lock);
        qput(input->queue->qp, (void *)elements[i]);
        input->queue->num_elements++;
        pthread_mutex_unlock(&q_lock);
        //printf("Item %d placed in queue\n", i);
        //fflush(stdout);
        // END CRITICAL SECTION

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
    CruncherInput_t *input = (CruncherInput_t *)args;
    // Wait loop
    while (!(output.calc_complete && !input->queue->num_elements)) {
        // CRITICAL SECTION
        // Grab element
        pthread_mutex_lock(&q_lock);
        qe = (QueueElement_t *)qget(input->queue->qp);
        // END CRITICAL SECTION

        if (qe == NULL) {
            pthread_mutex_unlock(&q_lock);
            sleep(CRUNCHER_WAIT_TIME);
            continue;

        }
        else {
            input->queue->num_elements--;
            //printf("Item taken from queue, queue now holds %d\n", input->queue->num_elements);
            //fflush(stdout);
            pthread_mutex_unlock(&q_lock);

        }
        // Crunch division
        val = integrate(qe->fp, qe->a, qe->b, qe->p, 0, 0, &f_depth);
        
        // Grab lock
        pthread_mutex_lock(&a_lock);
        // CRITICAL SECTION
        // Add result to accumulator
        output.accumulator += val;
        output.depth += f_depth;
        pthread_mutex_unlock(&a_lock);
        // END CRITICAL SECTION
    }
    return NULL;
}
