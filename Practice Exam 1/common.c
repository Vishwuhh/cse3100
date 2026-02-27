// Do not modify starter code
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define MAX_LEN 100

// arr[][MAX_LEN] is a 2D array of characters. In C, this is how we store an array of strings.
// 'n' is the total number of strings passed into the function.
void commonChars(char arr[][MAX_LEN], int n) {
  
  // THE GLOBAL RECORD:
  // This array keeps track of the "survivors". 
  // If common[0] is true, it means 'a' has been found in EVERY string so far.
  int common[26];
  
  // We start with extreme optimism: assume EVERY letter from 'a' to 'z' is common to all strings.
  for (int i = 0; i < 26; i++) {
    common[i] = true;
  }

  // Loop through each individual string in our array of strings.
  for (int i = 0; i < n; i++) {

    // THE LOCAL RECORD:
    // We create a fresh, blank boolean array for the CURRENT string we are looking at.
    // '{false}' initializes all 26 slots to false automatically.
    bool temp[26] = {false};
    
    // Find out how many characters are in the current string so we know how far to loop.
    int len = strlen(arr[i]);
    
    // Read the current string, character by character.
    for (int j = 0; j < len; j++) {
        // HOW THE ASCII MATH WORKS:
        // Characters in C are just integers under the hood. 'a' is exactly 97. 'b' is 98.
        // arr[i][j] grabs the current character (let's say it's 'c', which is 99).
        // 99 - 97 = 2. 
        // We go to temp[2] (which represents 'c') and set it to true!
        temp[arr[i][j] - 'a'] = true; 
    }
    
    // THE ELIMINATION PHASE:
    // Now we compare our Local Record (temp) against the Global Record (common).
    for (int k = 0; k < 26; k++) {
        // If a letter does NOT exist in the current string (temp[k] == false)...
        if (temp[k] == false) {
            // ...then it is mathematically impossible for it to be common across ALL strings.
            // We strike it from the Global Record. Once it becomes false here, it can NEVER become true again.
            common[k] = false; 
        }
    }
  }

  printf("Common characters: ");

  bool found = false;
  
  // THE OUTPUT PHASE:
  // Check the Global Record one last time. Whatever is still 'true' survived the elimination gauntlet.
  for (int k = 0; k < 26; k++) {
      if (common[k] == true) {
          // HOW THE REVERSE ASCII MATH WORKS:
          // If k is 2, it means 'c' survived. 
          // 2 + 'a' (which is 97) = 99. 
          // %c tells printf to print the ASCII character for 99, which outputs "c".
          printf("%c", k + 'a');
          
          // We flag that we found at least one character so we don't accidentally print "None".
          found = true;
      }
  }

  // If the 'found' flag was never tripped to true, the Global Record was completely wiped out.
  if (!found) {
    printf("None");
  }
  printf("\n");
}

// ... (main function remains exactly the same as previous output)