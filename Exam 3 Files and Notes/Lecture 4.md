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
    - consumer is eager and tries to consume before the producer can produce anythingj 
