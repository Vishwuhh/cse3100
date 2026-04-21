# Threads: Resource Sharing
```C
#include <pthread.h>
// compiles and links with '-pthread'
int pthread_create(
    pthread_t* thread, 
    pthread_attr_t* attr,
    void*(*start_routine)(void*),
    void* arg);

// terminating itself or returning from start_routine()
void pthread_exit(void *retval);

// avoids zombie threads (like zomie processes)
int pthread_join(
    pthread_t thread,
    void **retval);
```
## Virual Address Space with *Multiple* Threads
- each thread gets a stack in the memory
  - stacks have a max size to keep stacks seperated --> avoids overlap
  - *the global data is shared*
## 32-bit Implication 
- virtual address space on a 32-bit OS
  - linux = 2G
  - windows = 1G
- typical stack size/thread
  - 8 mb/thread
- memory usage
  - executable = 1-50 mb
  - heap = 1-200 mb
  - stacks = (# of threads)*8
    - 100 threads leads to 800 mb
## Circumventing the Limit?
- we can use smaller stacks, but *beware of recursions*
- seperate tasks in several processes and connect them with pipes or shared VM
- use a 64-bit OS
  - enough room left for 2^40 stacks after partitioning 
## Advantages of Threads
- simpler programing model, and is *easier to coordinate*
- lighter than processes
  - takes less resources to manage
- threads improves performance when given substantial CPU and I/O resources
### Light Weight
- lower overhead for thread creation
- lowe context switching overhead
- less OS resources used
### Shared State
- simpler programing model, and doesnt need IPC-like tools for interthread communication
## Shared Data: global, heap, and local
```C
int a[100];
// a is defined outside of functions, so can be accessed by all threads

int main(void) {
    int i; // stack
    char*p = malloc(1000); // the heap
    void*arg = &i;
    // passes p or arg to threads
}
```
### Example: Array Sum
- use two threads to compute the sum of ints in an array
## Disadvantages of Theads
- if a thread does something illogical, the *entire process crashes and burns*
- sharing resources could also be very unsafe
```C
long count = 0;

void* increase(void *arg) {
    long i, inc = *(long *)arg;
    for(i = 0; i < inc; i++>) {
        count++;
    }
    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
    pthread_t tid1, tid2;
    long in = ato1(arg >= 2 ? argv[1] : "100");
    pthread_create(&tid1, NULL, increase, &inc);
    pthread_create(&tid2, NULL, increase, &inc);
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    printf("counter is %ld\n", count);
    return 0;
}
```
- consider two threads each doung the same process
  - the addition becomes more complex in assembly, and the threads execute *concurrently*
    - this can be interrupted even on a single core, though
## Lesson
- even sharing a single **integer** can mess up the threads
  - need coordinations!
    - rules and protocols
    - establishes how to share data safely and keep everyone happy 
## Road Ahead
- define PROTOCOLS and DATA STRUCTURES to share
- examples
  - mutex/spinlocks
  - conditions
  - semaphores
  - barriers
  - producer/consumer
  - reader/writer
  - etc.
## How Unsafe?
- *subtle issues*
  - it can be very easy to get wrong
- good multi-threading programming practices need to be followed
  - good code, practice, synchronization, and low overhead

## Detaching a Thread
```C
int pthread_detach(pthread_t tid)
```
- parent thread doesn't need to wait
- threads can detach other threads, and when a detached thread terminates, its resources are released
```C
pthread_detach(pthread_self())
```
- threads creates as **joinable** and not detached can be joined

## Passing Arguments to Threads - 1
```C
#define NUM_THREADS 8

struct thread_data{
  int thread_num;
  char* message;
};

void* PrintHellp(void* threadarg) {
  struct thread_data* my_data = (struct thead_data*) threadarg;

  sleep(1 + 5*(my_data -> thread_num % 2));

  printf("thread #%d: %s length=%zd\n", my_data -> thread_num, my_data -> message);

  pthread_exit(NULL);
}
```