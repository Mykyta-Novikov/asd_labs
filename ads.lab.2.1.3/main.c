#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <string.h>

int useRecursionDepth = 0;
unsigned long long recursionDepth;

double compute (double x, int i, double previous);

int main(int argc, char* argv[]) {
    double x;
    printf("Input x: ");
    scanf("%lg", &x);

    if (argc >= 2)
        for (int i = 0; i < argc - 1; i++)
            if (strcasecmp(argv[i], "-i") == 0) {
                recursionDepth = strtoull(argv[++i], NULL, 10);
                useRecursionDepth = 1;
            }


    double result = compute(x, 1, x) + x;

    printf("x = %.*g", DBL_DECIMAL_DIG, result);
    return 0;
}

/**********************************************************************************************************************/

double compute (double x, int i,  double previous) {
    double f = previous * x*x / (4*i*i + 2*i);



    if (!useRecursionDepth && f == 0 ||
            useRecursionDepth && i >= recursionDepth)
        return f;
    else {
        double sum = compute(x, i + 1, f) + f;

        if (sum == INFINITY || sum == -INFINITY) {
            puts("sh(x) exceeds double boundaries");
            exit(-1);
        }

        return sum;
    }
}
