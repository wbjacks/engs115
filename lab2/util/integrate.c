// Static function prototypes
static double f1(double t);
static double f2(double t);
static double f3(double t);
static double f4(double t);

// Includes
#include "../util/integrate.h"

// Global
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
ThreadOutput_t output;

double integrate(double (*fp)(double t), double a,
    double b, double p, double f_sprev, int depth, int *final_depth)
{
    // Check against previous interations to see if precision has been reached
    double val = (fp)(a) * (b - a) + 0.5 * ((fp)(b) - (fp)(a)) * (b - a);
    double ret;
    if (depth != 0 && fabs(f_sprev - val) < p) {
        ret = val; // Base case
        *final_depth = pow(2, depth);

    }
    // Else, recurse on half of each value
    else {
        ret = integrate(fp, a, a + 0.5*(b - a), p, val, depth+1, final_depth)
            + integrate(fp, a + 0.5*(b - a), b, p, val, depth+1, final_depth);

    }
    return ret;

}

ThreadOutput_t m_integrate(int func_number, double a, double b, double p, int m) {
    // Construct threads
    if (m < 1)
        m = 10;

    pthread_t threads[m];
    ThreadInput_t *inputs[m];
    int i;
    void *status;

    // Prepare output
    output.accumulator = 0.0;

    // Launch
    for (i = 0; i < m; i++) {
        inputs[i] = (ThreadInput_t *)malloc(sizeof(ThreadInput_t));
        inputs[i]->start = a + i * ((b - a) / (double)m);
        inputs[i]->end = a + (i+1) * ((b - a) / (double)m);
        inputs[i]->p = p;
        inputs[i]->func_number = func_number;
        pthread_create(&threads[i], NULL, launcher, (void *)inputs[i]);
        //printf("Pthread %d launched!\n", i);

    }

    // Join
    for (i = 0; i < m; i++) {
        pthread_join(threads[i], &status);

    }

    // Accumilate values
    //printf("Accumulated value of the integral from %f to %f is: %f", a, b, output.accumulator);
    return output;
}

void *launcher(void *args) {
    ThreadInput_t *input = (ThreadInput_t *)args;
    double (*fp)(double t);
    double val;
    int final_depth;

    switch (input->func_number) {
        case 1: fp = f1; break;
        case 2: fp = f2; break;
        case 3: fp = f3; break;
        case 4: fp = f4; break;
        default:
            fprintf(stderr, "Error: improper function input to m_integrate, exiting.\n");
            return NULL;

    }
    val = integrate(fp, input->start, input->end, input->p, 0, 0, &final_depth);

    // Incremenet accumulator
    pthread_mutex_lock(&lock);

    // CRITICAL SECTION, lock should be acquired
    output.accumulator += val;
    output.section_count += final_depth;
    pthread_mutex_unlock(&lock);

    // END CRITICAL SECTION

    // Return- what is the difference between return and pthread_exit()?
    //pthread_exit();
    return NULL;
}

static double f1(double t) {
    return t+5;

}

static double f2(double t) {
    return 2*t*t + 9*t + 4;

}

static double f3(double t) {
    return t*t*t;

}

static double f4(double t) {
    return 3*powf(2, t);

}
