#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include "../util/integrate.h"

double func(double t);

int main(int argc, char *argv[]) {
    double result;
    double a, b, p;
    int depth_result;
    if (argc == 1) {
        a = 2.0;
        b = 4.0;
        p = 0.1;

    }
    else {
        a = atof(argv[1]);
        b = atof(argv[2]);
        p = atof(argv[3]);

    }
    printf("Hello, world!\n");
    result = integrate(func, a, b, p, 0, 0, &depth_result);
    printf("Result is: %f, depth is: %d\n", result, depth_result);
    return EXIT_SUCCESS;

}

// F(t) = t^3
double func(double t) {
    return t*t*t;

}
