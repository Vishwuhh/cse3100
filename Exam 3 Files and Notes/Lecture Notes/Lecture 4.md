## Lecture Dump
- mutex review
  - one has to get the lock before entering the critical section
    - would yeild exclusive access to source
    - if a thread cannot get the lock, it waits until it can
  - two threads sharing data
    - thread 1 would compute the result, and 2 would display it on the UI once computation is done
## *how does thread 2 know the computation is done?*
  - classic problem! producers put things into a shared buffer and the consumers take them out
    - mutual exclusion causes one thread to manipulate the buffer at a time --> avoids information loss
    - consumers must *wait* if the buffer is empty, and producers must *wait* if the buffer is full

```C
do {
    lock the buffer // buffer is shared, so it might have to wait
    if(buffer not empty) {
        fetch data from buffer
    }
    unlock puffer
} while(data is not fetched) // try in loop since it might not fail
process the data
```
- if the buffer is empty, the lock-unlock would be infinite and is very expensive 

```C
while(! nElements > 0) { // wait while buffer is empty
    // wait for someone to tell me data is ready
}
```
- predicate/condition
  - logic expression that describes the property of an object the program needs

- POSIX conditon variable
  - second and last core synchronization primitive
  - paired with MUTEX
  - if predicate is not true, thread can wait on condition variable 

- tips 
  - mutex protects
  - the conditional variable
    - locks mutex before waiitng
      - mutex is unlocked while waiitng
      - automatically and atomically rechecks the mutex when waking up
  - predicate P is checked in while loop
    - while loop returns to waiitng when P is false 
    - done need pthread_mutex_unlock() in it when doing cond_wait()

- two scenarios
  - scenario 1
    - producer gets to produce before the consumer consumes the previous one
  - scenario 2
    - consumer is eager and tries to consume before the producer can produce anything

## Producer-Consumer with Bounded Buffer
- classic problem: producers put things into a shared buffer, and consumers take them out
- constraints
  - mutual exclusion
    - only **one thread** can manipulate the buffer at any time
      - **BUFFER IS SHARED!!**
  - synchronization
    - consumers must wait if buffer is empty
    - producers must wait if buffer is full
  - *can this be done with only a lock?*
### Solution-ish
- consumer behavior is pseudo-code
```C
do {
  lock buffer // buffer is shared, so may need to wait
  if(buffer is Not empty) {
    fetch data from buffer
  }
} while(data is not fetched) // this may fail, so try adding into loop

process data
```

- consumer behavior in c-like code
```C
// consumer fetches data from a buffer, one at a time
int fetched = 0;

do {
  pthread_mutex_lock(&buffer_lock); // gets the lock for the buffer
  if(nElements > 0) { // if buffer is not empty
    data = fetch_from_buffer(); // fetches data from buffer
    fetched += 1; // increments fetched
  }
  pthread_mutex_unlock(&buffer_lock);
} while(fetched == 0)
// process data
```
- *what happens if the buffer is empty?*
  - program would never run due to failing if-statement
  - program needs a condition variable to determine if there is usable data
- consumer waits on *condition*
```C
pthread_mutex_t buffer_lock;
// consumer fetches data from a buffer one at a time
pthread_mutex_lock(&buffer_lock); // gets lock for the buffer

while(!nElements > 0) { // waits if the buffer is empty
  // waits for someone telling me the data is ready
}

data = fetch_from_buffer(); // fetches data from buffer
pthread_mutex_unlock(&buffer_lock);

// processes data
```
- consumer waits on condition variable - alternative method
```C
// uses while loop, not an if
// contains predicate

pthread_mutex_lock(&buffer_lock);
// check predicate when mutex is locked, mutex is unlocked while waiitng, and locked when waiting is over
while(! nElements > 0) {
  // waits for signal telling data is ready
}
data = fetch_from_buffer();
pthread_mutex_unlock(&buffer_lock);
```
## POSIX condition variable
- second and last **core** synch. primitive
  - all other primitives can be implemented with mutexes and condition vars
- always paired with a mutex, since its access needs to be **mutex protected**
- if a predicte is false, a thread can wait on a *condition variable*
  - other threads can *signal* on the variable when the predicate *changes*
    - threads must still check if the predicate is true b/c of potential false wake-ups
### pthread_cond API
```C
#include <pthread.h>

pthread_cond_t cond; // declaring a conditional variable

int pthread_cond_init(pthread_cond_t *cond, const pthread_condattr_t *cond); 
int pthread_cond_destroy(pthread_cond_t *cond);
int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex);
int pthread_cond_signal(pthread_cond_t *cond); 
int pthread_cond_broadcast(pthread_cond_t *cond);
```
## Tips
- mutex protects the shared data and the predicate
- the condition variable locks the mutex before waiting 
  - mutex is unlocked automatically while waiting 
  - also automatically and atomically relocks the mutex when waking up
- predicate is checked in a while loop
  - returns to waiting when the predicate is false 
## Structure of Using MUTEX and Condition
```C
// defining the mutex and the conditional variable
pthread_mutex_t mutex;
pthread_cond_t cond;

pthread_mutex_lock(&mutex); // retrieves mutex, and locks
while(!predicate) { // checks predicate if it false
  pthread_cond_wait(&cond, &mutex); // waits on condition, mutex is unlocked
  do_something();
}
access_shared_resources(); // safe to access shared resources
pthread_mutex_unlock(&mutex);

// can continue with other tasks, and access_shared_resources() should be **short**
```
## Signal vs. Broadcast
```C
// wakes up *one waiting thread* in the condition
pthread_cond_signal(pthread_cond_t *cond);
// wakes up *all* waiitng threads in the condition
// only one thread can grab mutex, rest sleeps if they fail
pthread_cond_broadcast(pthread_cond_t *cond);
```
- caveat
  - thread calling wait *after* broadcast will not wake up
## *Producer* in Pseudo-code and C-like Code
- peudo-code
```C
prepare data

do {
  lock buffer // may need to wait
  if (buffer is not full) { // bounded buffers
    put data into buffer
  }
  unlock buffer
} while(data is not placed in buffer)
```
- c-like code
```C
pthread_mutex_t buffer_lock;
// producer adds data into buffer one at a time
... // prepares data

added = 0;
do { 
  pthread_mutex_lock(&buffer_lock); // gets lock for buffer
  if(nElements < BUF_SIZE) { // if buffer isnt full
    add_to_buffer(data); // adds data to buffer
    added = 1
  }
  pthread_mutex_unlock(&buffer_lock);
} while(added == 0);
```
- *what happens if buffer is full?*
  - producer will not add elements into the buffer, and will only get the lock for the mutex
