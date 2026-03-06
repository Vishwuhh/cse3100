#include <stdio.h>
#include <stdlib.h>

double two_d_random(int n)
{
	//When deciding which way to go for the next step, generate a random number as follows.
	//r = rand() % 4;
	//Treat r = 0, 1, 2, 3 as up, right, down and left respectively.

	//The random walk should stop once the x coordinate or y coordinate reaches -n or n. 
	//The function should return the fraction of the visited (x, y) coordinates inside (not including) the square.
	
	// 1. Array Mapping Setup
	// A grid spanning from -n to n has a total width of 2n + 1.
	int size = 2 * n + 1;
	
	// In C99, we can declare a Variable Length Array (VLA) using our calculated size.
	int visited[size][size];
	
	// Initialize the grid to 0 to prevent garbage data.
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			visited[i][j] = 0;
		}
	}
	
	// 2. Initialize the Walker
	// The walker starts at the mathematical origin (0, 0).
	int x = 0, y = 0;
	
	// To safely access the array, we shift the origin to the center by adding 'n' to both coordinates.
	visited[x + n][y + n] = 1; 
	int unique_visited = 1;
	
	// 3. The Walk Loop
	// Continue walking while the walker is STRICTLY inside the -n and n borders.
	while (x > -n && x < n && y > -n && y < n) {
		
		int r = rand() % 4; // Generate direction
		
		// Update coordinates based on direction rules
		if (r == 0) y++;      // Up
		else if (r == 1) x++; // Right
		else if (r == 2) y--; // Down
		else if (r == 3) x--; // Left
		
		// Check if the NEW coordinate is still strictly inside the boundaries
		if (x > -n && x < n && y > -n && y < n) {
			
			// If this specific array slot is marked 0, we have never been here before.
			if (visited[x + n][y + n] == 0) {
				visited[x + n][y + n] = 1; // Mark it visited
				unique_visited++;          // Increment our unique counter
			}
		}
	}
	
	// 4. Calculate the Final Fraction
	// The interior area of the square (excluding the outer walls at -n and n) 
	// is a square of width (2n - 1) and height (2n - 1).
	double total_inside = (2.0 * n - 1.0) * (2.0 * n - 1.0);
	
	// Cast to double to ensure floating point division
	return (double)unique_visited / total_inside;
}

//Do not change the code below
int main(int argc, char *argv[])
{
	int trials = 1000;
	int i, n, seed;
	if (argc == 2) seed = atoi(argv[1]);
	else seed = 12345;

	srand(seed);
	for(n=1; n<=64; n*=2)
	{	
		double sum = 0.;
		for(i=0; i < trials; i++)
		{
			double p = two_d_random(n);
			sum += p;
		}
		printf("%d %.3lf\n", n, sum/trials);
	}
	return 0;
}
