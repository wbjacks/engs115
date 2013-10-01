#include <stdlib.h>
#include <math.h>

float integrate(float (fp)(float t), float a, float b, float p, float f_sprev) {
    // Check against previous interations to see if precision has been reached
    float val = (fp)(a) * (b - a) + 0.5 * ((fp)(b) - (fp)(a)) * (b - a);
    float ret;
    if (f_sprev != 0 && fabs(f_sprev - p) )
        ret = val; // Base case

    // Else, recurse on half of each value
    else
        ret = integrate(fp, a, a + 0.5*(a - b), p, val) + integrate(fp, a+0.5*(a-b), b, p, val);

    return ret;
}
