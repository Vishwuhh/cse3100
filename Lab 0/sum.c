/*
  Compute the sum of the integers
  from 1 to n, for a given n    
*/
#include <stdio.h>

int main(void) {
    /* Task from Lab 0:
       Calculate the sum of positive even integers below 200.
       Expected output: 9900
    */
    printf("Hello, World!\n");

    int even_sum = 0;
    int j = 2; 

    while (j < 200) {
        even_sum += j;
        j += 2; 
    }
    printf("%d\n", even_sum);
  
    /* int i, n, sum;
    sum = 0;
    printf("Enter n:\n");
    scanf("%d", &n);
    i = 1;
    while (i <= n) {
        sum = sum + i;
        i = i + 1;
    }
    printf("Sum from 1 to %d = %d\n", n, sum); 
    */

    // Corrected code segment: changed print to printf and updated N
    int N = 100000;
    printf("Among 1, 10, 100, 1000, 10000, 100000, 1000000\n");
    printf("The smallest number to break the code is %d\n", N);

    return 0;
}

