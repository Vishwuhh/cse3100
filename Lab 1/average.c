// Only this line of comment is provided 
#include <stdio.h>

int main(void) {
    double x; // declares all inputs as double
    double total = 0.0; // sum of all inputs
    double average = 0.0; // solution when dividing total by number of inputs
    int count = 0; // number of inputs

    while (scanf("%lf", &x == 1)) { // checks if the input is a double
        count += 1;
        total += x;
        average += total/count;
    }

    printf("Total=%f Average=%f\n", total, average); // prints the running total and average
}