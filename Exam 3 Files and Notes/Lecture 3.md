# Threads: Mutual Exclusion (mutex)
- learned how to create threads (pthread_create)
- learned how to terminate execution (pthread_exit)
- learned how to wait on threads (pthread_join)
- can carry out indepenedent computations with threads
  - sharing is a problem, though
## Mutual Exclusion
- protect shared resoruces, and have only one thread access them
- protocol
  - set of procedures for accessing shared resources
  - if thead 1 and 2 uses the same variable x
    - if thread 1 uses it first, the time it does is a *critical session*
    - needs to signal to thread 2 that it is done for it to use it
      - while thread 1 is in the critical session, thread 2 is sleeping until a signal from 1 is heard
## Race to Critical Section
- multiple threads race each other to get to the critical section
  - code segment that accesses shared resources
- one wins the race
  - winner executes the critical section first and releases the lock afterwards
  - loser waits until the resources is reallocated and then executes the section
## Mutex
- lock is a POSIX abstraction called the **mutex**
  - provided by operating system
- semantics
  - at most one thread can acquire the lock at a time
  - anyone else must sleep until the lock is released
## pthread mutex type and API
```C
// defines a mutex and shared var
// only one thread can lock it

pthread_mutex_t mutex;
```
- functions can
  - intialize the mutex
  - destroy a mutex we dont need
  - locks a mutex
  - unlocks a mutex
  - and attempt to lock a mutex 
## Initialization
```C
#include <pthread.h>

int pthread_mutex)init(pthread_mutex_t* mutex, const pthread_mutexattr_t* attr)
```
- initailizes a mutex from the OS and returns 0 on success
## Destruction
```C
int pthread_mutex_destroy(pthread_muthread_t* mutex)
```
- releases resources for the lock, returns 0 on success
## Lock/Unlock
```C
int pthread_mutex_lock(pthread_mutex_t* mutex) // enters crit session
int pthread_mutex_unlock(pthread_mutex_t* mutex) // leaves crit session
```
## Key Observations
- lock and unlock comes in a pair
- need one MUTEX per item to protect
  - packages integer and mutex together in the practice structs
  - DOES NOT PROTECT ANYTHING ELSE
- lock is **blocking**
- critical sections should be short, since locking is a **big cost**
## mutex_trylock()
```C
int pthread_mutex_trylock(pthread_mutex_t *mutex)
```
- if mutex is not locked, would succeed and lock
- if locked
  - will not block and will return error code
- purpose
  - allows polling before blocking, sometimes fine to use shared resource later
## Amdahl's Law
- Speedup = 1/[(1-p) + (p/n)] where n = CPU cores
## Mutex Attributes
- value of type *pthread_mutexattr_t*
  - optional argument to create mutex
- API is to
  - initalize attr record *pthread_mutexattr_init()*
  - destroy attr record *pthreaf_mutexattr_destroy()*
  - modify attr in attr record
    - set type, get type, etc.\
  - key property: mutex **type**
    -  how to deal with recursive lock? 
    -  what if other threads tries to unlock?
## Mutex Types - Documentation
- PTHREAD_MUTEX_**NORMAL**
  - **does not detect deadlock**
  - thread attempting to relock without unlocking will lead to deadlock
  - unlocking a mutex with different thread leads to undefined behavior
  - unlocking a unlocked mutex leads to undefined behavior
- PTHREAD_MUTEX_**ERRORCHECK**
  - **provides error checking**
  - thread attempting to relock without unloicking leads to error
  - thread attempting to unlock mutex that was locked by another threads leads to error
  - thread attempting to unlock an unlocked mutex leads to error
- PTHREAD_MUTEX_**RECURSIVE**
  - thread attempting to relock mutex without unlocking will lead to **successful relocking**
  - relocking deadlock in normal MUTEXs cannot happen with those defined as "recursive"
    - mutliple locks of this mutex needs same number of unlocks to release it before another thread can acquire it
  - thread attemping to unlock mutex when locked by another thread will lead to error
  - thread attempting to unlock an unlocked mutex will return an error 
