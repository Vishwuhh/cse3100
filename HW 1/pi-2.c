#include <stdio.h>
#include <stdlib.h>

int main()
{
	int n, i;

	printf("n = ");
	scanf("%d", &n);

	double pi = 0.0;
	// We need to calculate 1 / (16^i) for each iteration.
	// Since the homework forbids the pow() function, we maintain a running 'factor'.
	// It starts at 1.0 (which mathematically represents 1 / 16^0).
	double factor = 1.0; 
	
	for (i = 0; i <= n; i++) {
		
		// Calculate the inner formula: (4/(8i+1) - 2/(8i+4) - 1/(8i+5) - 1/(8i+6)).
		// CRITICAL EXAM NOTE: We MUST use 4.0, 2.0, and 1.0 here instead of integers 4, 2, and 1.
		// If we used plain integers, C would perform "integer division" and aggressively 
		// truncate all the decimal values to 0, completely breaking the sum.
		double term = (4.0 / (8 * i + 1)) - 
		              (2.0 / (8 * i + 4)) - 
		              (1.0 / (8 * i + 5)) - 
		              (1.0 / (8 * i + 6));
		
		// Multiply the evaluated term by our current 1/(16^i) factor and add it to the total pi sum.
		pi += term * factor;
		
		// Prepare the factor for the NEXT iteration of the loop (i + 1).
		// Dividing the current factor by 16.0 mathematically achieves 1 / (16^(i+1)).
		// This is how we simulate a power function efficiently.
		factor /= 16.0; 
	}

	printf("PI = %.10f\n", pi);
	return 0;
}
