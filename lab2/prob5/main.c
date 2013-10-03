#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include "../util/generator.h"

double func(double t);

int main(int argc, char *argv[]) {
    GeneratorOutput_t result;
    double a, b, p;
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
    result = generator(func, a, b, p, 10);
    printf("Result is: %f", result.accumulator);
    return EXIT_SUCCESS;

}

// F(t) = t^3
double func(double t) {
    return t*t*t;

}
