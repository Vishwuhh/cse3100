#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

void one_particle(int *grid, int n)
{
	// 1. Initialize Particle at the Origin
	int x = 0, y = 0, z = 0;

	// 2. The Random Walk
	// The particle takes exactly 'n' steps.
	for (int i = 0; i < n; i++) {
		int r = rand() % 6; // Generate a direction from 0 to 5
		
		// Move across the 3D axes based on the random number
		if (r == 0) x++;
		else if (r == 1) x--;
		else if (r == 2) y++;
		else if (r == 3) y--;
		else if (r == 4) z++;
		else if (r == 5) z--;
	}

	// 3. Coordinate Flattening (The 3D to 1D Map)
	// We shift the origin by adding 'n' so we don't have negative indices.
	// The width, height, and depth of the grid is (2n + 1).
	int size = 2 * n + 1;
	
	// We calculate the flat 1D index using standard dimensional offsets:
	// Index = X + (Y * Width) + (Z * Width * Height)
	int index = (x + n) + (y + n) * size + (z + n) * size * size;

	// Increment the particle count at this final resting coordinate
	grid[index]++;
}

double density(int *grid, int n, double r)
{
	// 1. Avoid math.h (Square Root Trap)
	// We need to find particles within distance r*n.
	// Euclidean distance is sqrt(x^2 + y^2 + z^2). 
	// To avoid sqrt(), we square BOTH sides of the inequality: (x^2 + y^2 + z^2) <= (r*n)^2.
	double max_dist_sq = (r * n) * (r * n);
	
	int total_particles = 0;
	int inside_particles = 0;
	int size = 2 * n + 1;

	// 2. Scan the entire 3D Grid space
	for (int x = -n; x <= n; x++) {
		for (int y = -n; y <= n; y++) {
			for (int z = -n; z <= n; z++) {
				
				// Map the current coordinates to the 1D array index
				int index = (x + n) + (y + n) * size + (z + n) * size * size;
				int count = grid[index];
				
				total_particles += count; // Tally up every particle we find
				
				// Check if this coordinate falls inside the spherical boundary
				if ((x * x + y * y + z * z) <= max_dist_sq) {
					inside_particles += count;
				}
			}
		}
	}
	
	// Force floating-point division
	return (double)inside_particles / total_particles;
}

void print_result(int *grid, int n)
{
    printf("radius density\n");
    for(int k = 1; k <= 20; k++)
    {
        printf("%.2lf   %lf\n", 0.05*k, density(grid, n, 0.05*k));
    }
}

void diffusion(int n, int m)
{
	// 1. Dynamic Allocation (Heap Memory)
	int size = 2 * n + 1;
	int total_cells = size * size * size;
	
	// CRITICAL EXAM NOTE: We use calloc instead of malloc here!
	// calloc automatically initializes all memory bytes to 0. If we used malloc, 
	// the grid would be full of garbage data and our particle counts would be wildly incorrect.
	int *grid = (int *)calloc(total_cells, sizeof(int));

	// Run the simulation for m particles
	for(int i = 1; i<=m; i++) one_particle(grid, n);

	print_result(grid, n);
	
	// 2. Memory Cleanup
	// If you malloc/calloc memory, you MUST free it, or your program leaks memory.
	free(grid);
}

// (The starter code main() function goes here)
