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

# Summary
- a = fork()
    - parent: a > 0 and is **equal to** child proccess id (PID)
    - child: a = 0

# Examples