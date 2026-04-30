# Threads 1: Intro and Basic Management
- motivation and concurrency
  - thread abstraction
- creation and termination

## Motivation
- processes run concurrently on an O.S. on multi-core systems 
  - single-core processors have timesharing to handle multiple tasks
- processes can also provide *protection*

- each process has its own *vritual space*
  - two processes would have their own space in a disk (from high to low)
  - think as if they are in their own building

- why? 
  - it is very safe to isolate each other
    - can't interfere, move their things, use their things, or break them

- downsides?
  - communication outside of the process is tricky
  - process cannot leave its area
  - has to do tasks *alone* and can't interact outside of its space
- all processes are equally isolated

## Threads?
- can invite other people into the isolated space 
  - moves independently, can do the tasks assigned to it, communicates *for* you, and can use their resources
- as powerful as the person who is the head of the space

## Refining Definitions 
  - process is a bundle grouping
    - virtual address space [memory]
    - collection of files and sockets [I/O]
    - collecion of *concurrent threads* [execution unit]
  - threads are very *lightweight*
    - runs concurrently with other threads 
    - shares resources with the process [having same rights]
    - confined to a *single* process [cannot move to another process]
    - can be created and destroyed [different life cycles]
  - *lightweight processes that share the address space in a process*

## Circle of Life  
- threads are created with *other threads*
  - pthread_create() 
  - given a stack to execute and a function to run
- what about the first thread?
  - process is created, there is a single thread
- how does a thread die?
  - either voluntarily with pthread_exit() *after task*
  - requested by another thread with pthread_cancel()
  - when the process dies, all associated threads die with it
    - any thread can call exit()
      - reminder: *exit()is only called when main() returns*
      - so, any thread can end the process

## pthreads API
- ANSI/IEEE POSIX 1003.1-1995 standard
- three possible routines
  - thread management: create, terminate, join, detach
  - mutexes: mutual exclusion, creating, destroying, locking, and unlocking mutexes
    - mutex = **mutual exclusion**
  - condition variable: event driven synchronization
### pthreads API and Naming
- naming conventions
  - pthread_ = general pthread
  - pthread_attr_ = thread attributes
  - pthread_mutex_ = mutual exclusion
  - pthread_mutexattr = mutual exclusion attributes
  - pthread_cond_ = condition variables
  - pthead_condattr = conditional variable attributes
  - pthread_key_ = *thread specific* data keys
```C
#include <pthread.h>
cc -pthread a.c // -pthread option is needed to compile in Linux
```
- pthread functions *do not* set errno on errors
  - many types are defined in pthread lib, but they are *opaque objects*
    - cannot make the assumptions on both representation/implementation
- example: pthread_equal() is used to compare two thread IDs

## main() Function of Thread
```C
void * thread_main(void * arg)
```
- "main" function of a thread
  - could be any name assigned
- takes a pointer for a parameter
  - points to int, char, string, or *a structure*
- returns a pointer
  - points to anything like the parameter, but **does not** point to local variables on stack

## Thread Creation
```C
int pthread_create(
    pthread_t* thread, // returned thread ID, if successful
    pthread_attr_t* attr, // specifices the attribute of thread, is NULL by default
    void* (*start_routine)(*void), // the "main" function of thread
    void* arg // passed to start_routine()
);
```
- returns 0 for a success, and non-zero (greater than zero) for errors
- the equivalent to fork() for threads, but it creates instead of clones
- threads are peers once created, and they can create other threads
  - no implied heiarchy or dependency between threads
  - **all threads are equal**

## What About Thread Concurrency?
- single-core CPU: timesharing
  - when one thread is waiting for an I/O to complete, another can use the CPU for computing
  - CPU is only used by *one thread* at a time
- multi-core CPU: true concurrency
  - MMID = multiple instructions, multiple data
  - threads execute on parallel, with one in each core
    - OS can preempt threads
  - useful when the number of threads is much larger than number of cores
    - timesharing is still used too!

## Thread Termination
- return from start_routine function or can call pthread_exit()
```C
void pthread_exit(void* status)
```
- function *always* succeeds and does not return value
- status is obtained through different threads
- similar to process termination
  - main() returns or exit() is called by *any* thread 

## Joining a Thread
```C
int pthread_join(pthread_t tid, void** status)
```
- wait for a thread to complete
  - blocks the calling thread until the thread id terminats
- obtains the exit status of the thread
  - NULL is used to ignore return value
- equivalent of waitpid() for processes

- joining is a simple way to do synchronization
  - calling thread can get the target threads return status if it was *specified* in target thread's call to pthread_exit()

## Question
```C
int main(void) {
    // creates 10 threads
    return 0;
}
```

## Passing Arguments and Getting Results Back
```C
#define NUM_THREADS  8

struct thread_data { // data structure 
  int thread_num; // integer to identify thread
  char* message; // string for the thread to process
  int len; // variable to store result of thread
};

void* PrintHello(void* threadarg) {
  struct thread_data* my_data = (struct thread_data*) threadarg; // casts argument as a *thread_data since it arrives as a *void
  sleep(1 + 5*(my_data -> thread_num % 2)); // sleep is staggered to simulate threads sleeping in different times

  // calculates the length of the assigned messge and prints status in console
  my_data->len = strlen(my_data->message);
  printf("Thread #%d: %s length=%d\n", my_data->thread_num, my_data->message, my_data->len);
  
  pthread_exit(NULL); // closes the threads in the process
}
```

## Common Ways to Use Threads
- pipeline
  - task is broken into sub-operations, and each are handles as a series
    - concurrently by a *different thread*
- manager/worker
  - single thread, manager assigns work to different threads (workers)
  - manager handles all input and parcels out work to the workers
  - two forms: static worker and dynamic worker pool
- peer
  - after main thread creates other threads, it participates in the work
  - similar to manager/worker model
## Applications of Threads
- parallel computing
  - threads can be executed at the same time with multi-core machines
- overlap CPU work with I/O
  - when one thread is waiting for an I/O, others perform CPU work
- asynchronous event handling
  - ex. web server handles both transfer data from previous requests and manage the arrival of new requests
- priority/real-time scheduling
  - important tasks scheduled with higher priority
- computer games
  - each threat controls the movement of objects
- etc...

## Why is it Hard for the Thread to do Everything By Itself?
- threads cannot do multiple things concurrently without affecting application performance
  - needs **multiple threads**
### Usage Examples
- we can create multiple threads, with one per sub stack
- ex. word processor
  - one controls the GUI
  - one reformats the document
  - one does background tasks, like saving and syncing changes
- worker threads could also do parallel computations
  - matrix-vector multiplication has all the rows done in parallel
  - we can also simulate agents in parallel (like agents in game)

## Classical Thread Model
- could be three processes with each with one thread **or** one process with three threads
- shared by all threads in a process
  - address space, global variables, open files, child processes, pending alarms, signals/signal handlers, accounting information
- private to each thread
  - program counter, registers, stack, and stake 