# More Thread Synchronization
## Deadlock
- state when no member can make progress
  - all are waiting for others to take actions
    - threads, processes, computers, etc. 
- example
  - two mutexes A and B
    - thread 1: locked A try to get B
    - thread 2: locked B try to get A
### Solutions
- fixed order
  - all threads lock mutexes in the *same order*
    - m0, m1, m2, ...
- try and back off
  - try to lock, if fails, release all locks and try again
## More Synchronization Primitives
- so far covered mutexes and condition variables
- now
  - read-write locks
  - barriers
- mutexes and condition variables are **primitive constructs**
- read-write locks and barriers can be built using mutexes and condition variables 
## Readers-Writers Problem
- a data structre is *read* frequently but *written* infrequently
- constraints
  - multiples readers and read simultaneously 
    - readers can start even when others are already doing so
  - only one writer can write at the same time 
  - read and write cannot happen at the same time
    - readers have to wait if a writer is writing
    - one writer has to wait for all readers and other writers to finish
### Solutions?
- use pthreads **read-write locks**
  - like a mutex with *two independent* lock functions (read/write)
- use mutex and condition variables
  - keep track of number of readers and writers
    - active and waiitng participants
  - choose proper predicate
    - if true, proceed to read/write
    - if false, wait until true
  - notify others when thread is done with read/write
## Pthread Read-Write Lock
```C
# include <pthread.h>
pthread_rwlock_t unlock; // defines read-write lock

// initialize and destroy a read-write lock
int pthread_rwlock_init(
    pthread_rwlock_t *restrict rwlock,
    const pthread_rwlockattr_t *restrict attr);
int pthread_rwlock_destroy(pthread_rwlock_t * rwlock);

// lock functions for readers and writers, and then unlocks 
int pthread_rwlock_rdlock(pthread_rwlock_t *rwlock);
int pthread_rwlock_wrlock(pthread_rwlock_t *rwlock);
int pthread_rwlock_unlock(pthread_rwlock_t *rwlock);
```
## Pthread Read-Write Locks for *Reading*
- first lock function locks for **reading**
  - thread gets read lock iff *no writer has the lock and no writer is blocked on the lock*
    - more than one thread can get this lock
  - only one writer is allowed at any time 
## Pthread Read-Write Locks for *Writing*
- second lock function locks for **writing**
  - thread gets write lock iff *no reader or writer has the lock*
    - must wait for the read lock too
  - only one writer is allowed at any time 
## Read-Write Lock Issues
- writer starvation
  - too many readers, and writers don't have a chance to start
- solution: "write locks take precedence over read locks"
  - leads to reader starvation
    - readers don't have a chance to read any of the written data
## Pthread Read-Write Lock
- pthread rwlock prefers writer according to manual page
## Use MUTEX and Condition to Implement rwlock
- using mutex and condition is more flexible, so we can adjust our strategy
  - best one: one mutex and two condition variables (one read, one write)
    - keep track of numbers needed for predicate
### Pseudocode for rwlock Lock/Unlock
- lock pseudocode
```C
pthread_mutex_lock(&mutex);
// increment/decrement waiting counter and while loop
increment waiting counter
// predicate depends on the policy
while(! predicate) {
  pthread_cond_wait(&cond);
}
decrement waiting counter
increment active counter
pthread_mutex_unlock(&mutex);
// rwlock is locked, now can start to read/write
// but, mutex is *unlocked*
```
- unlock pseudocode
```C
perform read/write operations // lock type dependent
// unlock rwlock when operation is done
pthread_mutex_lock(&mutex);
decrement active counter
// inform waiting threads
// policy decides if checking reader or writer first
if(writer waiting) {
  signal write cond
}
if(reader waiting) { // only needed for write unlock
  broadcast read cond
}
pthread_mutex_unlock(&mutex);
```
### Example: rwlock_writeunlock()
- does this lock prefer reader or writer?
## Barriers
- needed for applications where work is done in phases
  - must have worker threads wait for the whol group to be done before proceeding to next phase
    - workers put to sleep while waiting
  - number of workers done known prior
## POSIX Barrier Support
```C
#include <pthread.h>
// create barrier, but note the count argument
int pthread_barrier_init(
  pthread_barrier_t *restrict barrier,
  const pthread_barrierattr_t *restrict attr, 
  unsigned count
);
// destroy a barrier
int pthread_barrier_destroy(pthread_barrier_t *barrier);
// wait on a barrier
// when function returns, one thread gets PTHREAD_BARRIER_SERIAL_THREAD, others get 0
int pthread_barrier_wait(pthread_barrier_t *barrier);
```
## Sync Mechanisms in Course
- can use mutex, condition, read/write lock, barriers, and only call blocking functions
- following **cannot** be used for synchronization
  - busy waiting (ex. spin lock)
  - sleep functions