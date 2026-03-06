# Intro to Processes

## Proccess Basics
- process = instance of program being executed
    - core concept in operating systems
        - multiprocessing OS can have multiple programs happen at the same time **OR** multiple instances of the same program happening at the same time
- executing multiple programs
    - single-core uses time sharing
    - multi-core has parallelism and timesharing 

## Proccess Management: OS View
- OS's have a proccess table 
    - every proccess has a **proccess control block (PCB)**
- OS scheduler *picks proccess to execute* at any time
    - suspended proccesses are saved in PCB

## Paged Virtual Memory
- physical memory is **shared** across **all procceses**
- page table maps virutal addresses to physical addresses
    - multiple virtual pages can be mapped to the **same** physical pages

## Proccess Management: User's View
- events can cause proccess creation
    - system initialization
    - user request to create a new proccess (ex. **shell command**)
    - executing **shell script**, which can create many proccesses 
- events that can cause *termination*
    - normal program exit
    - error exit
    - fatal error (ex. segementation fault)
    - filled by user command or signal
        - Ctrl+C

## User Command
- ps
    - lists running proccesses
- pstree
    - shows the **tree** of proccesses
- top
    - dynamic view of the memory and CPU usage
        - also shows proccesses that use the most resources
        - press q to exit top
- kill
    - kills a process given **process ID**
    - -9 is an option to do a simple kill

## Proccess Management: Programmer's View
- process birth
    - new proccesses are made from other proccesses
    - process starts as a **clone** of its parent process
        - can **upgrade itself** to run a different executable as needed
            - can retain access to the files open in the parent 
- process life
    - can create its own child proccesses as well
- proccess death
    - can call **exit** or **abort** to end itself, or can get killed by user

## Birth via Cloning
```C
#include <unistd.h>
pid_t fork(void);
```
- function above is to create a **new process** in the code
- child is an exact copy of the parent
    - both return from fork()
- the difference is the *returned value*
    - parent proccess
        - fork() returns the proccess identifier of the child (>0)
            - **this is the proccess id of the process**
        - when falures happen, it returns -1 and sets the errno
    - child proccess
        - fork() returns **just zero**

## Concurrency
- **parent and child** proccess reutrn from fork concurrently
    - can return at the same tieme on a multicore machine, or one after the other
    - **cannot** assume they reuturn at the same time or which returns first -> order is chosen by the *OS scheduler*

## Cloning effect
- on memory
    - parent and child memory is 100% identitcal, but viewed as *distinct* according to OS
    - memory changes (stack/heap) only affects that copy -> parent and child can diverge
- on file
    - all files open in the parent are **accessible** to the child
    - i/o operations in one would move the file position indicator
        - **stdin, stdout, and stderr** of the parent is accessible to the children

## What can the parent do?
- depends on application
    - can wait until child is done, so it dies
        - shells like bash/ksh/zsh/csh
    - can run concurrently and check on child later *or* could ignore the child
        - when child dies, it becomes a **zombie**

### Waiitng on a child
```C
#include <sys/wait.h>
pid_t wait(int *status);
pid_t waitpid(pid_t pid, int *status, int options)
```
- purpose
    - blocks the calling proccess untl the child is terminated
        - also until other state changes happen
    - report status in *status
        - is ignored if NULL is passed
        - shows the cause of death and the exit status of the child 
    - return value identifies the child proccesses
- run "**man -S2 wait**" for the full details

### Zombies
- dead proccesses waiting to be checked by its parent
    - cannot be killed, and while it released *most* of its resources, it still has an entry in the proccess table 
- parents should check on kids
    - some systems can have parents not check on kids
- 'init' becomes the *new parent* if a parent dies to the kids
    - children are reaped afterwards

## System calls
- APIs used to request services from OS kernel
    - fork() 
    - system calls *are more expensive* than normal function calls 
        - manual calls are in section 2
        ```C
        man -S2 intro ; man -S2 syscalls
        ```

### Virtual Memory
- parent and child might have same information, but have **different memory locations**
    - stored in virtual memory
        - to safeguard actual RAM and memories
```C
#include <stdio.h>
#include <unistd.h>
int main() {
  int a = 0;
  if (fork() == 0) {
    a++;
    printf("Child process: a = %10d, &a = %p\n", a, &a);
  } else {
    a--;
    printf("Parent process: a = %10d, &a = %p\n", a, &a);
  }
  return 0;
}
```
- example of memory divergence
  - the child increments its local version of a, but the parent *decrements* its a
    - both originally had the same values
#### Paradox of the Memory Address (&a)
- the *hexadecimal address* is the same for both parent and child
  - address %p prints the **virtual address* instead of the physical RAM location
    - since the child is a clone, its virtual address is mapped identically to its parent
      - **they think that a is in the same spot on *both* maps**
  - OS page table would map these virtual addresses to **different physical locations** on the RAM 

| Process | Value of a | Memory Address (&a) |
| --- | --- | --- |
| Child | 1 | 0x7ffee123 |
| Parent | -1 | 0x7ffee123 |



# Summary
- a = fork()
    - parent: a > 0 and is **equal to** child proccess id (PID)
    - child: a = 0

# Examples
```C
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
  int a = fork(); // part 1
  int b = fork(); // part 2

  printf("a = %d,b=%d,pid = %d\n", a, b, getpid());
  if (a > 0 && b > 0) {
		waitpid(a,NULL,0);
		waitpid(b,NULL,0);
    printf("The main process 1, the god father\n");
  }
	if(a==0){
		if(b==0){
			printf("This is process 4\n");
		}
		if(b>0){
			printf("This is process 2\n");
		}
	}
	if(a>0 && b==0){
		printf("This is process 3\n");
	}	
  return 0;
}
```
- forking operations
  -  first fork()
     - for int a, clones the original proccess leaving **two processes** active
  - second fork()
      - for int b, clones the two processes from 1 into **four processes** in total
- proccess identification
  - Parent
    - fork() returns the PID of the *child*, which is a value > 0
  - Child
    - fork() returns zero
- waitpid()
  - ensures that process 1 stays active *until* the children finish
  
| Process | Variable a | Variable b | Description |
| --- | --- | --- | --- |
| Process 1 | > 0 (PID of P2) | > 0 (PID of P3) | Original Parent |
| Process 2 | 0 | > 0 (PID of P4) | First fork; parent of P4 |
| Process 3 | 0 (PID of P2) | 0 | Second fork from P1 |
| Process 4 | 0 | 0 | Second fork from P2 |

```C
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
  printf("This is a demonstration\n"); // beginning of code
  int a = fork();
  if (a == 0) {
    printf("This is in child process\n");
    printf("Child process id = %d\n", getpid());
  }
  if (a > 0) {
    printf("This is in parent process\n");
    waitpid(a, NULL, 0);
    printf("Parent process id = %d\n", getpid());
  }

  printf("Bye!\n");
  return 0;
}
```
- intialization
  - the program starts as a single process and prints "This is a demonstration"
- fork() call   
  - proccess clones itself, with both proccesses return from fork() **concurrently**
    - parent 
      - a contains the Process ID of the child (value > 0)
    - child 
      - a is exactly zero
  - child branch (when a == 0)
    - child enters the first if block
      - prints "this is in a child process" and its PID with *getpid()*
    - skips the if(a > 0) block and prints "Bye!" before exiting 
  - parent branch (when a > 0)
    - parent enters the second if block
      - prints "this is in parent proccess" 
      - waitpid(a, NULL, 0)
        - pauses at this line until the child process terminates
    - once child dies, the parent is unblocked
      - prints its PID and then prints "Bye!"

- key observations
  - **order of output** is sealed with waitpid() for the parent
    - ensures that the child's output is always first
    - would be *random* if waitpid() wasn't used 
  - while the child is a clone, because it is attached to a different virtual memory address, any changes to parent **only affects the parent**
  - parent is able to "reap" the child proccess, with helps stop the child from being a zombie
  
## Fork Treasure Hunt 
```C
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[]) {

  if (argc != 2) {
    printf("Usage: %s n\n", argv[0]);
    return -1;
  }

  int n = atoi(argv[1]);
  assert(n >= 1 && n <= 10);

  int *a = calloc(n, sizeof(int)); // allocates array a of size n with calloc (all elements are zero)

  srand(12345);
  int idx = rand() % n; // picks random idx and sets a[idx] = 1 (the treasure)
  pid_t pid = getpid(); 
  printf("Treasure hidden at %d in array %p pid = %d\n", idx, a, pid); // prints memory address of the array %p and the current proccess id getpid()
  a[idx] = 1;

  int cur = 0;

  for (int i = 0; i < n; i++) { // runs up to n times, with a fork() happening every increment of n
    pid_t pid;
    pid = fork();
    if (pid == 0) { // child proccess starts as an exact copy of parent 
      if (a[cur]) { // checks array at the current index a[cur]
        printf(
            "Systematic search found the treasure at %d in array %p pid = %d\n",
            cur, a, getpid()); // prints the message of success and stops loop
        break;
        ;
      }
      cur++; // increments cur and continutes to the next iteration and fork --> child has its own copy of the cur variable 
    } else {
      // wait for the child process to finish
      waitpid(pid, NULL, 0);
      int guess = rand() % n;
      if (a[guess]) {
        printf("Random search found the treasure at %d in array %p pid = %d\n",
               guess, a, getpid());
        break;
        ;
      }
    }
  }
  free(a);
  return 0;
}
```
- key takeaway
  - systematic search is performed through a **chain of descendants**
    - process 1 creates process 2 (checks index 0), ... , process n creates process (n+1) for index (n-1)
  