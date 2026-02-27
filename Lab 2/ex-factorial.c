/* This program should read a single integer from standard input then
 * compute its factorial. For example, if the user enters 5, the program should
 * output 120 because 1*2*3*4*5=120. */

#include <stdio.h>

int factorial(int n)
{
    if(n == 0) // base case since 0! will be one; stops the loop
        return 1;
    return n*factorial(n-1); // recursive case, runs the factorial() function until n=0
}

int main()
{
    int n, result;

    if (scanf("%d", &n) != 1) { // verifies if the input is not a letter or an EOF signal
        return 1;
    }
    
    if (n < 0) {
        // need to print an error string if the value of n is not a positive integer
        printf("Enter a non-negative number");
        return 0;
    }

    result = factorial(n); // sets results to the value of the recursive function
    printf("%d\n", result);
    return 0; // executed perfectly signal
}
