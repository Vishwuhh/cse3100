#include <stdio.h>

int main(void)
{
  unsigned int v = 19; // 19 in binary is 10011
  char parity = 0;
  // any non zero value would evaluate to TRUE for while loops in C
  // loop would continue until v has zero 1-bits left
  while(v){
    // ! is a logical NOT, and flips 0s to 1s and vice versa
    // we toggle the parity tracker everytime we find a 1-bit in the number
    parity = !parity; 
    // subtracting 1 from a binary number flips rightmost 1-bit to a zero, and turns all 0-bits to the right to 1s
    // 10100 - 1 = 10011
    // bitwise AND the original v with (v-1) leads to an erasure 
    // always flips the rightmost 1-bit to a zero
    v = v & (v - 1);
  }
  return 0;
}
