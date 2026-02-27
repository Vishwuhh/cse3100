//In this assignment, we implement a function that simulates the outcome of tossing of 3 dice.
//We will learn how to generate random numbers, and how to seed a random number generator.
//For example, we use the following code to generate a random number between 1 and 6.
// rand() % 6 + 1
//The following code set the seed of the random number generator to 12345.
// srand(12345);

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

double cum_prob(int k, long trials) {
    long count = 0;
    
    for (long i = 0; i < trials; i++) {
		// the plus 1 makes sure that the range is [1-6] instead of [0-5]
        int die1 = rand() % 6 + 1; 
        int die2 = rand() % 6 + 1;
        int die3 = rand() % 6 + 1;
        
        if (die1 + die2 + die3 >= k) { // number of times the rolls exceed a number
            count++; 
        }
    }
    
    return (double)count / trials; // forces the result to be a decimal (for probability)
}

//Do not change the following code.
int main()
{
	long n = 10000000;
	int k;

	printf("Enter k :");	
	scanf("%d", &k);
	assert(k>= 3 && k<=18);
	srand(12345);
	printf("P(sum of the 3 dice is at least %d) = %.5lf\n", k, cum_prob(k, n));
	return 0;
}
