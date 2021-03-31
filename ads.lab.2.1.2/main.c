#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <math.h>
#include <string.h>

const double PRECISION = 1e-5f;

int useRecursionDepth = 0;
unsigned long long recursionDepth;

double compute (double x, int reversedI, double* fPointer, int* maxIPointer);

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


    double result = compute(x, 0, NULL, NULL);

    printf("x = %.*g", DBL_DECIMAL_DIG, result);
    return 0;
}

/**********************************************************************************************************************/

double compute (double x, int reversedI, double* fPointer, int* maxIPointer) {
    double previous;
    int maxI;

    if (useRecursionDepth && reversedI >= recursionDepth ||                                 // use fixed recursion depth
        !useRecursionDepth && (x*x / (4*reversedI*reversedI + 2*reversedI)) <= PRECISION) { // or calculate current multiplier
                                                                                            // each time instead
        if (maxIPointer != NULL) *maxIPointer = reversedI;
        if (fPointer != NULL) *fPointer = x;
        return x;    }

    double sum = compute(x, reversedI + 1, &previous, &maxI);



    int i = maxI - reversedI;
    double f = previous * x*x / (4*i*i + 2*i);

    sum += f;

    if (sum == INFINITY || sum == -INFINITY) {
        puts("sh(x) exceeds double boundaries");
        exit(-1);
    }

    if (maxIPointer != NULL) *maxIPointer = maxI;
    if (fPointer != NULL) *fPointer = f;
    return sum;
}
