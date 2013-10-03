#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

#include "../util/integrate.h"

int main(int argc, char *argv[]) {
    double a, b, p;
    int fn;
    if (argc == 5) {
        a = atof(argv[1]);
        b = atof(argv[2]);
        p = atof(argv[3]);
        fn = atoi(argv[4]);

    }
    else {
        fprintf(stderr, "Improper number of arguments, exiting.\n");
        return EXIT_FAILURE;

    }
    printf("Value returned is: %f\n", m_integrate(fn, a, b, p, 10));
}
