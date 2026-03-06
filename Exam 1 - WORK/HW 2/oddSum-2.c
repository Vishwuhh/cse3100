#include <stdio.h>
#include <stdlib.h>

int oddSumHelp(int count, int bound, int value)
{
	// 1. The Success Base Case
	// If we need 0 more numbers (count == 0) and the remaining sum needed is 0 (value == 0), 
	// we have found a valid combination. We return 1 to signal success back up the recursive chain.
	if (count == 0 && value == 0) {
		return 1;
	}
	
	// 2. The Failure Base Cases
	// - If we need 0 more numbers but the value isn't 0 yet, we failed.
	// - If our value dropped below 0, we overshot the target sum.
	// - If the bound dropped to 0 or below, there are no more positive odd numbers to try.
	// If any of these are true, we return 0 to trigger a backtrack.
	if (count == 0 || value < 0 || bound <= 0) {
		return 0;
	}
	
	// 3. Recursive Branch 1: "Take It"
	// The problem requires us to prioritize the largest odd numbers.
	// We make a recursive call that assumes 'bound' IS part of the solution.
	// We decrement 'count' (we picked a number), subtract 2 from 'bound' (to get the next odd number),
	// and subtract the current 'bound' from the target 'value'.
	if (oddSumHelp(count - 1, bound - 2, value - bound)) {
		
		// If this branch returned 1, it means this path eventually succeeded!
		// We print the current bound as part of the winning sequence.
		printf("%d ", bound);
		return 1; // Pass the success signal up to the caller
	}
	
	// 4. Recursive Branch 2: "Leave It"
	// If the "Take It" branch returned 0, it means including 'bound' caused a failure deeper in the tree.
	// So, we skip the current 'bound'. We keep 'count' and 'value' exactly the same, 
	// but we subtract 2 from 'bound' to try the next smallest odd number.
	return oddSumHelp(count, bound - 2, value);
}

//Do not change the code below
void oddSum(int count, int bound, int value)
{
    	if(value <= 0 || count <= 0 || bound <= 0) return;
    
    	if(bound % 2 == 0) bound -= 1;

    	if(!oddSumHelp(count, bound, value)) printf("No solutions.\n");
	else printf("\n");
}

int main(int argc, char *argv[])
{
	if(argc != 4) return -1;

	int count = atoi(argv[1]);
	int bound = atoi(argv[2]);
	int value = atoi(argv[3]);

	//oddSum(12,30,200);
	//oddSum(10,20,100);
	//oddSum(20,20,200);
	oddSum(count, bound, value);
	return 0;
}