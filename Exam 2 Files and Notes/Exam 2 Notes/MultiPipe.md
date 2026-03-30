- unidirectional pipes
  - standard pipes in Linux/C are unidirectional
    - data goes out from write end into read end
  - need to make seperate pipes to transfer data back and forth
- sibling process topology
  - child1 = fork() makes the first child
   ```C
    if(child1 > 0) {
        child2 = fork();
    }
   ```
   - helps ensure that only the parent process forks the second time 
     - if child did this again, it would make a "grandchild" process
 - aggressive file descriptor pruning 
   - if there are N pipes, there would be 2N file descriptors open after forking processes
     - close() statements are important when either not using the FDs in the process or when you're done with them
 - ASCII arithmetic
   - char = 8-bit integers
   - out = (char)(((int)out)+1)
     - would change 'a' into 'b'
     - casts the character into an intger, and then advances by 1
 - execution flow
   - outwards
    - parent - reads command line argument (*argv[1]), which is 'a'
      - writes 'a' into pd2[1] and then blocks, and waiitng to read from pd5
    - child1 - reads 'a' from pd2[0] and increments it to 'b'
      - writes 'b' into pd3[0] and blocks, waiting to read from pd4
    - child2 - reads 'b' from pd3[0] and increments it to 'c'
      - writes 'c' into pd5[1] and blocks, waiting to read from pd[6]
  - inwards
    - parent - reads 'c' from pd5[0] and increments to 'd'
      - writes 'd' into pd6[1] and blocks, waiting to read from pd1
    - child2 - reads 'd' from pd6[0] and increments to 'e'
      - writes 'e' into pd4[1] and calls exit(1) to terminate
    - child1 - reads 'e' from pd4[0] and increments to 'f' 
      - writes 'f' into pd1[1] amd calls exit(1) to terminate
    - parent - reads 'f' from pd1[0] and prints the final result before terminating
- wait()
  - wait is needed to catch the exit statues of child1 and child2, so they dont become zombies
```C
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <ctype.h>
#include <stdlib.h>

/*
=============================================================================
ROUTING SEQUENCE OVERVIEW:
Parent → Child1 → Child2 → Parent → Child2 → Child1 → Parent

1. Parent sends 'a' to Child1  (via pd2)
2. Child1 reads 'a', changes to 'b', sends to Child2 (via pd3)
3. Child2 reads 'b', changes to 'c', sends to Parent (via pd5)
4. Parent reads 'c', changes to 'd', sends to Child2 (via pd6)
5. Child2 reads 'd', changes to 'e', sends to Child1 (via pd4)
6. Child1 reads 'e', changes to 'f', sends to Parent (via pd1)
7. Parent reads 'f' and finishes.
=============================================================================
*/

int main(int argc, char **argv) {
    
    // Ensure an argument was provided
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <character>\n", argv[0]);
        exit(-1);
    }

    // --- 1. PIPE INITIALIZATION ---
    // We need 6 distinct one-way pipes to handle every direction of travel.

    int pd1[2]; // pd1: Child 1 -> Parent
    if (pipe(pd1) < 0) { perror("Error."); exit(-1); }

    int pd2[2]; // pd2: Parent -> Child 1
    if (pipe(pd2) < 0) { perror("Error."); exit(-1); }

    int pd3[2]; // pd3: Child 1 -> Child 2
    if (pipe(pd3) < 0) { perror("Error."); exit(-1); }

    int pd4[2]; // pd4: Child 2 -> Child 1
    if (pipe(pd4) < 0) { perror("Error."); exit(-1); }

    int pd5[2]; // pd5: Child 2 -> Parent
    if (pipe(pd5) < 0) { perror("Error."); exit(-1); }

    int pd6[2]; // pd6: Parent -> Child 2
    if (pipe(pd6) < 0) { perror("Error."); exit(-1); }

    // --- 2. PROCESS CREATION (SIBLING TOPOLOGY) ---
    
    int child1 = fork(); // Parent creates Child 1
    if (child1 < 0) { perror("Fork 1 failed."); exit(-1); }
    
    int child2 = 2; // Placeholder initialization
    
    // By wrapping the second fork in this if-statement, we guarantee 
    // that ONLY the Parent process creates Child 2. This prevents Child 1 
    // from accidentally spawning a "grandchild" process.
    if (child1 > 0) {
        child2 = fork();
        if (child2 < 0) { perror("Fork 2 failed."); exit(-1); }
    }

    // =========================================================================
    //                            CHILD 1 LOGIC
    // =========================================================================
    if (child1 == 0) { 
        
        // --- FILE DESCRIPTOR PRUNING ---
        // Child 1 only needs to read from Parent (pd2), write to Child 2 (pd3),
        // read from Child 2 (pd4), and write to Parent (pd1).
        // EVERYTHING else must be closed to prevent deadlocks.
        close(pd1[0]); // Child 1 doesn't read from pd1
        close(pd2[1]); // Child 1 doesn't write to pd2
        close(pd3[0]); // Child 1 doesn't read from pd3
        close(pd4[1]); // Child 1 doesn't write to pd4
        close(pd5[0]); close(pd5[1]); // Completely unused by Child 1
        close(pd6[0]); close(pd6[1]); // Completely unused by Child 1

        char out;

        // FIRST PASS: Receive from Parent, send to Child 2
        read(pd2[0], &out, sizeof(char)); // Blocks until Parent writes
        close(pd2[0]); // Best practice: close immediately after final use
        printf("\nChild1 read:%c\n", out);
        
        out = (char)(((int)out) + 1); // Cast to int, increment ASCII value, cast back
        
        write(pd3[1], &out, sizeof(char)); // Send to Child 2
        close(pd3[1]);
        printf("\nChild1 wrote:%c\n", out);

        // SECOND PASS: Receive from Child 2, send back to Parent
        read(pd4[0], &out, sizeof(char)); // Blocks until Child 2 writes
        close(pd4[0]); 
        printf("\nChild1 read:%c\n", out);
        
        out = (char)(((int)out) + 1); 
        
        write(pd1[1], &out, sizeof(char)); // Send final output to Parent
        close(pd1[1]);
        printf("\nChild1 wrote:%c\n", out);

        exit(1); // Child 1 terminates
    }

    // =========================================================================
    //                            CHILD 2 LOGIC
    // =========================================================================
    if (child2 == 0) {
        
        // --- FILE DESCRIPTOR PRUNING ---
        // Child 2 only needs to read from Child 1 (pd3), write to Parent (pd5),
        // read from Parent (pd6), and write to Child 1 (pd4).
        close(pd3[1]); 
        close(pd4[0]); 
        close(pd5[0]); 
        close(pd6[1]); 
        close(pd1[0]); close(pd1[1]); // Completely unused
        close(pd2[0]); close(pd2[1]); // Completely unused

        char out;

        // FIRST PASS: Receive from Child 1, send to Parent
        read(pd3[0], &out, sizeof(char)); // Blocks until Child 1 writes
        close(pd3[0]);
        printf("\nChild2 read:%c\n", out);
        
        out = (char)(((int)out) + 1);
        
        write(pd5[1], &out, sizeof(char)); // Send to Parent
        close(pd5[1]);
        printf("\nChild2 wrote:%c", out); // Note: missing \n in original code here

        // SECOND PASS: Receive from Parent, send back to Child 1
        read(pd6[0], &out, sizeof(char)); // Blocks until Parent writes
        close(pd6[0]);
        printf("\nChild2 read:%c\n", out);
        
        out = (char)(((int)out) + 1);
        
        write(pd4[1], &out, sizeof(char)); // Send to Child 1
        close(pd4[1]);
        printf("\nChild2 wrote:%c\n", out);

        exit(1); // Child 2 terminates
    }

    // =========================================================================
    //                            PARENT LOGIC
    // =========================================================================
    
    // --- FILE DESCRIPTOR PRUNING ---
    // The Parent only needs to write to Child 1 (pd2), read from Child 2 (pd5),
    // write to Child 2 (pd6), and read from Child 1 (pd1).
    close(pd1[1]); 
    close(pd2[0]); 
    close(pd5[1]); 
    close(pd6[0]); 
    close(pd3[0]); close(pd3[1]); // Completely unused by Parent
    close(pd4[0]); close(pd4[1]); // Completely unused by Parent

    // Fetch the starting character from the command line argument
    char out = *argv[1];

    // ROUND 1: Kickoff - Send to Child 1
    write(pd2[1], &out, sizeof(char));
    close(pd2[1]);
    printf("\nParent wrote:%c\n", out);

    // ROUND 2: Middle phase - Receive from Child 2, send back to Child 2
    read(pd5[0], &out, sizeof(char)); // Blocks until Child 2's first pass finishes
    close(pd5[0]);
    printf("\nParent read:%c\n", out);
    
    out = (char)(((int)out) + 1);
    
    write(pd6[1], &out, sizeof(char)); // Send back down to Child 2
    close(pd6[1]);
    printf("\nParent wrote:%c\n", out);

    // ROUND 3: Final phase - Receive from Child 1
    read(pd1[0], &out, sizeof(char)); // Blocks until the entire chain is finished
    close(pd1[0]);
    printf("\nParent read:%c\n", out);


    // --- ZOMBIE PROCESS PREVENTION ---
    // Now that the communication ring is complete, the Parent must wait
    // for both children to formally exit before the Parent process terminates.
    // This cleans up their entries in the OS process table.
    int status;
    waitpid(child1, &status, 0);
    waitpid(child2, &status, 0);

    return 0;
}
```

Write a C program that takes a starting integer from the command line. The program must create a Parent process and two sibling child processes (Child A and Child B).

The integer must be routed through the processes and modified according to this exact sequence:
- Parent reads the integer from argv[1], adds 10, and sends it to Child A.
- Child A receives the number, multiplies it by 2, and sends it to Child B.
- Child B receives the number, subtracts 5, and sends it back to the Parent.
- Parent receives the number, divides it by 3, and sends it to Child B. 
- Child B receives the number, adds 100, and sends it to Child A.
- Child A receives the number, multiplies it by 10, and sends it to the Parent.
- Parent receives the final number, prints it to the console, and formally waits for both children to exit.

```C
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

void die(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <starting_integer>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // --- 1. CREATE ALL 6 PIPES ---
    int p_to_a[2], a_to_b[2], b_to_p[2], p_to_b[2], b_to_a[2], a_to_p[2];

    if (pipe(p_to_a) < 0 || pipe(a_to_b) < 0 || pipe(b_to_p) < 0 || 
        pipe(p_to_b) < 0 || pipe(b_to_a) < 0 || pipe(a_to_p) < 0) {
        die("Pipe creation failed");
    }

    // --- 2. CREATE SIBLING PROCESSES ---
    pid_t childA = fork();
    if (childA < 0) die("Fork A failed");

    pid_t childB = 1; // Placeholder
    if (childA > 0) { // Only Parent forks again
        childB = fork();
        if (childB < 0) die("Fork B failed");
    }

    // =========================================================================
    //                              CHILD A LOGIC
    // =========================================================================
    if (childA == 0) {
        // --- FD Pruning ---
        // Keeps: p_to_a[0], a_to_b[1], b_to_a[0], a_to_p[1]
        close(p_to_a[1]); 
        close(a_to_b[0]); 
        close(b_to_p[0]); close(b_to_p[1]); // Unused by A
        close(p_to_b[0]); close(p_to_b[1]); // Unused by A
        close(b_to_a[1]); 
        close(a_to_p[0]);

        int val;

        // Step 2: Receive from Parent, * 2, Send to B
        read(p_to_a[0], &val, sizeof(int));
        close(p_to_a[0]);
        printf("Child A received: %d. Multiplying by 2.\n", val);
        val *= 2;
        write(a_to_b[1], &val, sizeof(int));
        close(a_to_b[1]);

        // Step 6: Receive from B, * 10, Send to Parent
        read(b_to_a[0], &val, sizeof(int));
        close(b_to_a[0]);
        printf("Child A received: %d. Multiplying by 10.\n", val);
        val *= 10;
        write(a_to_p[1], &val, sizeof(int));
        close(a_to_p[1]);

        exit(EXIT_SUCCESS);
    }

    // =========================================================================
    //                              CHILD B LOGIC
    // =========================================================================
    if (childB == 0) {
        // --- FD Pruning ---
        // Keeps: a_to_b[0], b_to_p[1], p_to_b[0], b_to_a[1]
        close(p_to_a[0]); close(p_to_a[1]); // Unused by B
        close(a_to_b[1]);
        close(b_to_p[0]);
        close(p_to_b[1]);
        close(b_to_a[0]);
        close(a_to_p[0]); close(a_to_p[1]); // Unused by B

        int val;

        // Step 3: Receive from A, - 5, Send to Parent
        read(a_to_b[0], &val, sizeof(int));
        close(a_to_b[0]);
        printf("Child B received: %d. Subtracting 5.\n", val);
        val -= 5;
        write(b_to_p[1], &val, sizeof(int));
        close(b_to_p[1]);

        // Step 5: Receive from Parent, + 100, Send to A
        read(p_to_b[0], &val, sizeof(int));
        close(p_to_b[0]);
        printf("Child B received: %d. Adding 100.\n", val);
        val += 100;
        write(b_to_a[1], &val, sizeof(int));
        close(b_to_a[1]);

        exit(EXIT_SUCCESS);
    }

    // =========================================================================
    //                              PARENT LOGIC
    // =========================================================================
    
    // --- FD Pruning ---
    // Keeps: p_to_a[1], b_to_p[0], p_to_b[1], a_to_p[0]
    close(p_to_a[0]);
    close(a_to_b[0]); close(a_to_b[1]); // Unused by Parent
    close(b_to_p[1]);
    close(p_to_b[0]);
    close(b_to_a[0]); close(b_to_a[1]); // Unused by Parent
    close(a_to_p[1]);

    int val = atoi(argv[1]); // Convert command line string to integer

    // Step 1: Parent starts, + 10, Send to A
    printf("Parent starting with: %d. Adding 10.\n", val);
    val += 10;
    write(p_to_a[1], &val, sizeof(int));
    close(p_to_a[1]);

    // Step 4: Receive from B, / 3, Send to B
    read(b_to_p[0], &val, sizeof(int));
    close(b_to_p[0]);
    printf("Parent received: %d. Dividing by 3.\n", val);
    val /= 3;
    write(p_to_b[1], &val, sizeof(int));
    close(p_to_b[1]);

    // Step 7: Receive final value from A
    read(a_to_p[0], &val, sizeof(int));
    close(a_to_p[0]);
    printf("\n>>> FINAL VALUE: %d <<<\n\n", val);

    // --- Clean up Zombies ---
    int status;
    waitpid(childA, &status, 0);
    waitpid(childB, &status, 0);

    return 0;
}
```