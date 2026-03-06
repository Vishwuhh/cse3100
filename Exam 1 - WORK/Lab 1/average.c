// Only this line of comment is provided 
#include <stdio.h>

int main(void) {
    // need to initialize all variables before use!!
    double x; // declares all inputs as double
    double total = 0.0; // sum of all inputs
    double average = 0.0; // solution when dividing total by number of inputs
    int count = 0; // number of inputs

    // needs the "%lf" (long float) to read into the double 
    while (scanf("%lf", &x) == 1) { // checks if the input is a double
        // updates running metrics
        count += 1;
        total += x;
        average = total/count;

        // need %f to print as a double, and /n for printing a new line at the end
        printf("Total=%f Average=%f\n", total, average); // prints the running total and average
    }
    return 0;
}