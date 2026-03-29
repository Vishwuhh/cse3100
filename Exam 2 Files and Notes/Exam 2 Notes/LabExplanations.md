## Lab 5 Explanation
- fork() and memory
  - complete duplicate of the calling process
    - all **open** file desciptors, variables, and stack frames are coped
  - pid_parent = child_pid (a)
  - pid_child = 0
  - parent can have a wait()/waitpid() function added so it waits for the child to finish its process
- exec family
  - different uses based on different needs
    - if needing full path
      - execl, execbv, execle, execve
        - e is for *custom environments*
    - if $PATH is given
      - uses p suffix (execlp, execvp)
        - execlp is for lists --> execkp(prog, arg0, arg1, NULL)
          - execlp is for whem argcount is *fixed* at the time of compiling
        - execvp is for arrays --> execvp(rpog, char*argv[])
          - exevp is for when args are in an array or if the count of args is varied
- WIFEXITED and WEXITSTATUS
  - WEXITSTATUS returns garbag is a signal killed the process
## Lab 6 Explanation
- Unidirectional Pipes 
  - pipes in the kernel are fixed-size circular buffers with two FD endpoints
    - data could only flow in one direction
    - write() blocks when buffer is full (>= 64KB written)
      - waits for read end to read the data
    - read() blocks when buffer is *empty* and the write end is open
      - waits for the write end to add data to pipe
    - read() returns 0 when buffer is empty and *all write ends are closed*
  - forces there to be two pipes (one for reading from child to parent, and writing from parent to child) 
## Multi-Pipe Operations
- shell pipeline (cmd1 | cmd2 | cmd3)
  - connects N commands where each commands STDOUT feeds into the next commands STDIN
    - needs N-1 pipes and to manage the FDs
    - cmd1: STDIN (terminal), STDOUT(p1[1])
      - p1[1] is the write end of the first pipe
      - closes p1[0], p2[0], and p2[1]
      - STDIN gets input from the screen
    - cmd2: STDIN (p1[0]), STDOUT (p2[1])
      - p1[0] is the read end of the first pipe
      - p2[1] is the write end of the second pipe
      - closes p1[1] and p2[0]
    - cmd3: STDIN (p2[0]), STDOUT (terminal)
      - p2[0] is the read end of the second pipe
      - closes p1[0], p1[1], and p2[1]
      - STDOUT prints to the screen
```C
// ls | grep .c | wc -l  (3 commands, 2 pipes)

int p1[2], p2[2];
pipe(p1);
pipe(p2);

// ── Child 1: runs "ls" ──
if (fork() == 0) {
    dup2(p1[1], 1);          // stdout → pipe1
    close(p1[0]); close(p1[1]);
    close(p2[0]); close(p2[1]);  // close ALL other pipe ends
    execlp("ls", "ls", NULL);
    perror("execlp"); exit(1);
}

// ── Child 2: runs "grep .c" ──
if (fork() == 0) {
    dup2(p1[0], 0);          // stdin ← pipe1
    dup2(p2[1], 1);          // stdout → pipe2
    close(p1[0]); close(p1[1]);
    close(p2[0]); close(p2[1]);
    execlp("grep", "grep", ".c", NULL);
    perror("execlp"); exit(1);
}

// ── Child 3: runs "wc -l" ──
if (fork() == 0) {
    dup2(p2[0], 0);          // stdin ← pipe2
    close(p1[0]); close(p1[1]);
    close(p2[0]); close(p2[1]);
    execlp("wc", "wc", "-l", NULL);
    perror("execlp"); exit(1);
}

// ── Parent: must close ALL pipe ends ──
// If parent keeps any write end open, that child's reader
// will never see EOF and will hang forever
close(p1[0]); close(p1[1]);
close(p2[0]); close(p2[1]);

// Wait for all three children
wait(NULL); wait(NULL); wait(NULL);
```
- Fan-Out (One writer, multiple readers)
  - need to send data from the parent to N children
    - having one shared pipe is **wrong**  
      - each byte is consumed by one reader, so the children races to read it so the distribution is *unpredictable*
    - each child needs their own pipe so the parent controls what each of them gets
  - fan-in(N writers, one reader) makes it so that a single pipe is fine
    - writes can share one pipe safely 
      - kernel makes sure writes is less than the pipe_buf

- Ring of Operations
  - We are given a ring: Parent to Child 1 to Child 2 to Parent
    - this must the done twice such that a value is multiplied through the ring with different values
      - Run 1: multiples are 2, 4, and 6
      - Run 2: multiples are 2, and 4.
```C
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

void die(const char *s) { // kill a process if error occurs
    perror(s);
    exit(EXIT_FAILURE);
}

int main() {
    // 0 is the read, and 1 is the write
    int p_to_c1[2] 
    int c1_to_c2[2]
    int c2_to_p[2]; // the pipes

    if(pipe(p_to_c1) == -1 || pipe(c1_to_c2) == -1 || pipe(c2_to_p) == -1) { // || is OR
        die("Pipe Failed"); // error message if the pipe failed
    }

    pid_t pid1 = fork() // first child
    if(pid1 < 0) {
        die("fork 1 failed") // if the forking failed
    }

    if(pid1 == 0) { // child 1 proccess
        // does not use the Child 2 to Parent pipe at all
        close(c2_to_p[0]);
        close(c2_to_p[1]);

        close(p_to_c1[1]); // Child 1 only reads from the parent
        close(c1_to_c2[0]); // Child 1 only wrotes to Child 2

        int val;
        // recives data from parent, multiplies by 2, and sends to child 2
        // this is for round 1
        if(read(p_to_c1[0], &val, sizeof(int)) > 0) {
            val *= 2;
            write(c1_to_c2[1], &val, sizeof(int));
        }
        // same implementation as above for round 2
        if(read(p_to_c1[0], &val, sizeof(int)) > 0) {
            val *= 2;
            write(c1_to_c2[1], &val, sizeof(int));
        }

        // closing used ends, and exiting
        close(p_to_c1[0]);
        close(c1_to_c2[1]);
        exit(EXIT_SUCCESS);
    }

    pid_t pid2 = fork(); // second child
    if(pid2 < 0) {
        die("fork 2 failed"); // error if the forking failed
    }

    if(pid2 == 0) {
        // close unused pipes
        close(p_to_c1[1]);
        close(p_to_c1[0]);

        // close unused ends
        close(c1_to_c2[1]); // will only read from child 1
        close(c2_to_p[0]) // will only write to the parent

        int val;
        // recives from child 1 to child 2, and if it is greater than zero, multiplies it by 4
        if(read(c1_to_c2[0], &val, sizeof(int)) > 0) {
            val *= 4;
            write(c2_to_p[1], &val, sizeof(int)); // writes the new value to the parent
        }
        // repeats this for round 2
        if(read(c1_to_c2[0], &val, sizeof(int)) > 0) {
            val *= 4;
            write(c2_to_p[1], &val, sizeof(int)); // writes the new value to the parent
        }
        // closes used ends and exits
        close(c1_to_c2[0]);
        close(c2_to_p[1]);
        exit(EXIT_SUCCESS);
    }

    // PARENT PROCESS
    // does not use the child pipe
    close(c1_to_c2[0]);
    close(c1_to_c2[1]);

    // close unused ends
    close(p_to_c1[0]); // parent only writes to child 1
    close(c2_to_p[1]); // parent only reads from child 2

    int val = 1; // the starting value
    printf("Parent starting with value: %d\n", val);

    // round 1: sending initial value to c1
    write(p_to_c1[1], &val, sizeof(int));
    // round 1: recieve from child 2, multiply by 6, and send back value to child 1
    if(read(p_to_c2[0], &val, sizeof(int)) > 0 ) {
        printf("Parent recived %d after first loop, will multiply by 6.\n", val);
        val *= 6;
        write(p_to_c1[1], &val, sizeof(int));
    }

    // round 2: recive final value from child 2
    if(read(p_to_c2[0], &val, sizeof(int)) > 0) {
        printf("Final value has been recived: %d\n", val);
    }
    // close the used pipe ends
    close(p_to_c1[1]);
    close(p_to_c2[0]);
    // wait for the child processes
    wait(NULL);
    wait(NULL);
    // return val
    return 0;
}
```
- Mathematical Sequence
  - parent starts with 1
  - child 1 doubles it to 2, parent is paused until child 2 is done
  - child 2 recives child 1's value, multiplied by 4 to 8 
  - child 2 gives the value to the parent, is multiplied to 48 and then waits again for child 2
  - given to child 1, and is doubled to 96
  - value is given to child 2 and then is multiplied to 384
  - parent recives the final value, and is them printed