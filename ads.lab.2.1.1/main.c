#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <math.h>
#include <string.h>

int useRecursionDepth = 0;
unsigned long long recursionDepth;

double compute (double x, int i, double previous, double sum);

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


    double result = compute(x, 1, x, x); // previous is f for i == 0 that equals x

    printf("x = %.*g", DBL_DECIMAL_DIG, result);
    return 0;
}

/**********************************************************************************************************************/

double compute (double x, int i,  double previous,  double sum) {
    double f = previous * x*x / (4*i*i +2*i);

    sum += f;

    if (sum == INFINITY || sum == -INFINITY) {
        puts("sh(x) exceeds double boundaries");
        exit(-1);
    }



    if (!useRecursionDepth && f == 0 ||
        useRecursionDepth && i >= recursionDepth)
        return sum;
    else
        return compute(x, i + 1, f, sum);
}
