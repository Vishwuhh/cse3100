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

# MANPAGE: POSIX Threads
## Thread Management
```C
int pthread_create(
  pthread_t *thread,
  const pthread_attr_t *attr,
  void *(*start_routine)(void*),
  void *arg
);
```
- spawns a *new, concurrent* execution thread
- arguments
  - attr: attributes, NULL for defaults
  - start_routine: function pointer to code the thread needs to execute
    - must return void* and take *exactly* one void* arg
  - arg: single pointer passed to start_routine, points to a struct containing thread-speicific data
- returns 0 on success and any other number when *error*
```C
int pthread_join(
  pthread_t thread,
  void retval
);
```
- suspends execution of calling thread until thread terminates
- arguments
  - thread: ID of thread to wait for
  - retval: pointer to a location where exit status of target thread is stored, NULL if you don't need return value
## Mutexes (Mutual Exclusion)
```C
int pthread_mutex_init(
  pthread_mutex_t *restruct mutex,
  const pthread mutexattr_t *restrict attr
);
```
- initializes a hardware-backed lock to prevent data races
  - data races = multiple threads trying to reach critical code segments at the same time 
```C
int pthread_mutex_lock(
  pthread_mutex_t *mutex
);
```
- blocks calling thread until mutex is available, then lcoks it
  - if locked by another thread, the caller sleeps
```C
int pthread_mutex_unlock(
  pthread_mutex_t *mutex
);
```
- releases lock on calling thread
```C
init pthread_mutex_destroy(
  pthread_mutex_t *mutex
);
```
- destroys mutex object, freeing OS resources
  - needs to be *unlocked* before destroying
## Condition Variables 
```C
int pthread_cond_wait(
  pthread_cond_t *restrict cond,
  pthread_mutex_t *restrict mutex
);
```
- unlocks the mutex and puts the thread to sleep on the cond queue
  - when woken up, thread *automatically re-acquires* the mutex before returning to the function
    - needs to always be wrapped in a while loop to check state condition b/c of **spurious OS wakeups**
```C
int pthread_cond_signal(
  pthread_cond_t *cond
);
```
- wakes up *at least* one thread waiting on the specified conditon variable
# MANPAGE: POSIX Sockets
## Setup and Resolution
```C
int getaddrinfo(
  const char *node,
  const char *service,
  const struct addrinfo *hints,
  struct addrinfo res
);
```
- translates the hostname (ex. "localhost") and service names (ex. 3119) into a linked list of structure addrinfo for creating sockets
- arguments
  - node: IP address or hostname
  - service: port number or service name
  - hints: points to a previous addrinfo structure specifying criteria
    - AF_UNSEPEC for IPv4/IPv6 agnostic, SOCK_STREAM for TCP
  - res: pointer to the head of the linked list
## Core Socket API
```C
int socket(
  int domain,
  int type,
  int protocol
);
```
- creates endpoint for communication, returns a *file descriptor*
- arguments
  - sockwt(p->ai_family, p->ai_socktype, p->ai_protocol) from data in getaddrinfo
```C
int bind(
  int sockfd, 
  const struct sockaddr *addr,
  socklen_t addrlen
);
```
- server assigns the address specified by addr to the socket refered to by sockfd
```C
int listen(
  int sockfd,
  int backlog
);
```
- server marks the socket as *passive*, used to accept incoming connection requests
  - backlog defines the max length of the queue of pending connections can be
```C
int accept(
  int sockfd, 
  struct sockaddr *restrict addr,
  socklen_t *restrict addrlen
);
```
- server extracts first connection request on queue, creates a new *connected* socket, and returns new FD for that socket
  - original sockfd remains *open* for future comms
```C
int connect(
  int sockfd, 
  const struct sockaddr *addr,
  socklen_t addrlen
);
```
- client initiates connection on socket to a specified address
## Data Transmission
```C
ssize_t send(
  int sockfd,
  const void *buf,
  size_t len,
  int flags
);
```
- transmits message to another socket
  - returns number of bytes actually sent when done 
    - **can be less than value of len** or -1 (only when error)
```C
ssize_t recv(
  int sockfd,
  void *buf,
  size_t len,
  int flags
);
```
- recieves message from a *connected* socket, and returns bytes read
  - if 0 is returned, peer performed an orderly shutdown (disconnected)