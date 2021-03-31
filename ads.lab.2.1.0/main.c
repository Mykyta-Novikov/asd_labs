#include <stdio.h>
#include <float.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

int useRecursionDepth = 0;
unsigned long long recursionDepth;

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


    int i = 1;
    double f = x, sum = x;

    do {
        f *= x*x / (4*i*i +2*i);
        sum += f;
        i++;

        if (sum == INFINITY || sum == -INFINITY) {
            puts("sh(x) exceeds double boundaries");
            return -1;
        }
    } while (!useRecursionDepth && f != 0 ||
            useRecursionDepth && i <= recursionDepth);

    printf("sh(x) = %.*g", DBL_DECIMAL_DIG, sum);
    return 0;
}