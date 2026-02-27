#include <stdio.h>
#include <stdlib.h>

int main()
{
	int n;

	printf("n = ");
	scanf("%d", &n);

	int m = n;
	
	// A happy number eventually reduces to 1.
	// The prompt states an unhappy number will ALWAYS fall into an endless cycle that includes the number 4.
	// Therefore, we keep looping as long as 'n' hasn't hit either of our two exit conditions.
	while (n != 1 && n != 4) {
		
		// Print the current value of n before we break it down, as required by the output format.
		printf("%d\n", n); 
		
		int sum = 0;
		
		// We use a temporary variable so we can destroy it while extracting digits, 
		// without losing our actual outer loop condition variable too early.
		int temp = n;
		
		// This inner loop peels off digits from right to left until no digits are left (temp drops to 0).
		while (temp > 0) {
			
			// Modulo 10 gives us the remainder of dividing by 10, which is always the right-most digit.
			// Example: 145 % 10 = 5.
			int digit = temp % 10;
			
			// Square the extracted digit and add it to our running total for the next cycle.
			sum += digit * digit;
			
			// Divide by 10 to shift the integer right, permanently chopping off the last digit.
			// Example: 145 / 10 = 14 (because C integer division drops the .5 remainder).
			temp /= 10;
		}
		
		// Overwrite 'n' with the newly calculated sum of squares so the outer loop can check it.
		n = sum;
	}
	
	// Print the final 1 or 4 that triggered the loop to stop.
	printf("%d\n", n); 

	if(n==1) printf("%d is a happy number.\n", m);
	else printf("%d is NOT a happy number.\n", m);
	return 0;
}
