#include <stdio.h>

/* This function calculates n-th Catalan number.
 * n is assumed to be non-negative.
 * Do not worry about overflow.
 * Do not print anything in this function.
 * Do not use floating-point numbers (or operations).
 */
long catalan_number(int n)
{
    if (n < 0) {
        return 0;
    }

    long cat_num = 1; // base case since C_0 is one
    int k = 1; // sets k = 1 for loop
    
    // could also be a for-loop if int k isnt defined BEFORE the loop
    // for(int k = 1, k <= n, k++)
    while(k <= n) { // goes up to n to build sequence
        cat_num = ((cat_num * (4*k - 2)/(k+1)));
        k++;
    }

    return cat_num;
}

/* do not change main function */
int main(void)
{
    int n;

    while (scanf("%d", &n) == 1) {
        if (n < 0) 
            printf("C(%2d) is not defined.\n", n);
        else 
            printf("C(%2d)=%18ld\n", n, catalan_number(n));
    }
    return 0;
}
