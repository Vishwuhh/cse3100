## Exam Review Session 1
- what is a process?
  - any program in execution
  - IDEA: running mutlitple programs/processes cocncurrently 
  - good things
    - uses virtual memory, one process can't corrupt other processes memory
  - bad thing
    - is a copy of *entire memory space*
    - SOLUTION: create threads to do the specific task inside the process
      - individual pieces of code
- every program has one main thread
  - can create other threads
    - pthread_t t1 is the *thread identifier*
    - pthread_create(&t1, ...) creates thread from identifier 
    - void* fn(void *arg) is a thread function
      - to send multiple elements, put them in arrays or structures
      - int id = * (int*)(arg) to **dereference pointer**
    - pthread_exit(NULL) exits thread if thread doesn't pass a result
      - replace NULL with the successful output if the thread passes something
      - NOTE: pass something on the **heap**, not the **stack**
    - pthread_join(t1, result) where int*result is a second argument based on what the thread passes
      - waits on t1 and t2 to finish before joining them
      - makes sure that the process doesn't end once the program runs through the lines of code
- mutex = MUTUAL EXCLUSION
  - makes sure to define shared resources such that one thread can access it at a time 
  - two threads will race to the shared course to change it
    - leads to *race condition*, and the order is not solidified
  - SOLUTION: define mutex
    - initialize the mutex
    - define the critical code
      - code between the lock condition and unlock condition
      - think of what you need to have good performance --> SMALL AS POSSIIBLE
        - *what if theres different shared vars?* = use multiple mutexes
          - lets other threads do different tasks since the mutexes make sure there wont be repeats
```C
// where should you put a mutex?
pthread_mutex_t t1
int a[] = {10, 20, 30};
void *fl(void *arg) { // YOU DONT NEED TO IF ITS ALONE. the variable is not being manipulated
    int idx = *(int*)arg;
    // if not alone, do the LOCK AND UNLOCK
    pthread_mutex_lock(&t1); // b/c there is another function, we need to have the lock for race conditions 
    print(a[idx]);
    pthread_mutex_unlock(&t1); 
    pthread_exit(NULL);
}
// what about for a modifying function?
void * f2(void *arg) {
    int idx = *(int*)arg;
    // we need an lock here
    pthread_mutex_lock(&t1); // same mutex since same variable
    a[idx] ++;
    // we need an unlock here
    pthread_mutex_unlock(&t1); 
    pthread_exit(NULL);
}
```
- producer and consumer problem
  - shared buffer
  - count variable is also a shared variable
    - needs to be protected with a mutex to avoid changes
  - producers add values into buffer, consumers takes it from the buffer
  - can lead to "busy waiting" --> program is waiting, but it is still running and taking up CPU space 
    - reason for having the conditional variables, it *avoids* the busy waiting
  - the predicate is where the check for empty stack
    - is locked because the *predicate* is a shared resource
  - consumer, when woken up, will run through the loop AGAIN
