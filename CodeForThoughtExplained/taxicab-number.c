//In this assignment, we will find the smallest positive integer that 
// can be expressed as a sum of two positive cube numbers in two distinct ways.
// More specifically, we want to find the smallest n such that n == i1*i1*i1 + j1*j1*j1, 
// n == i2*i2*i2 + j2*j2*j2, and (i1, j1) and (i2, j2) are not the same in the sense that
// not only (i1, j1) not euqal to (i2, j2), but also (i1, j1) not equal to (j2, i2).
// We practice using loops in this assignment.
// Also, we practice how to do an early exit in a loop. We exit the loop once we find the smallest n. 
// Once we are done, we will be delighted surprised to find out that this number already played a big role in 
// our computer science study.
#include <stdio.h>
int main()
{
	int i, j, n;

	for(n=1; n<=1000000; n++) // assumes smallest n is no larger than 1000000
	{
		int count = 0; // records the number of different numbers
		for (i = 1; i * i * i < n; i++) { // bound the i loop
		    for (j = i; i * i * i + j * j * j <= n; j++) { // start j at i to prevent duplicates amd bound j so it stops when sum exceeds n
		        if (i * i * i + j * j * j == n) { // increments count only if i^3 + j^3 is equal to n
		            count++;
		        }
		    }
		}
		if (count >= 2) { // shatters loop
		    break; // Early exit once we find the smallest n
		}
	}


	//Do not change code below
    printf("%d is the solution:\n", n);
	for(i=1; i<=100; i++)
		for(j = i; j<= 100; j++)
			{
				if (i*i*i + j*j*j == n)
				{
					printf("%d * %d * %d + %d * %d * %d = %d\n", i, i, i, j, j, j, n);
                }
			}

	return 0;
	//Do not try to hard code the solution, that is not the way to learn.
}
