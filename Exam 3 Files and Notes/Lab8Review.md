## Lab 8 Review
- guess-mt.c
```C
// Thread C main function
void *thread_c(void *arg_in) {
    thread_arg_t *arg = arg_in;
    gmn_t gmn;
    gmn_init(&gmn, arg->seed);
    my_msleep(rand());
    // lock shared structure
    pthread_mutex_lock(&arg->mutex);
    // send max value to thread p
    arg->max = gmn_get_max();
    arg->status = S_MAX;
    pthread_cond_signal(&arg->cond_result); // to wake up thread P

    int result = -1;
    while(result != 0) {
        // waits for guess from thread_p
        // USE WHILE LOOP to hand OS wakeups
        while(arg->status != S_GUESS) {
            pthread_cond_wait(&arg->cond_guess, &arg->mutex); 
        }

        result = gmn_check(&gmn, arg->guess); // calls gmn_check()
        arg->result = result; // sends result to thread_p

        // if guess is correct, copy final message
        if(result == 0) {
            strcpy(arg->message, gmn_get-Message(&gmn));
        }

        // signal to thread p that result is ready
        arg->status = S_RESULT;
        pthread_cond_signal(&arg->mutex);
    }
    // unlock, then exit
    pthread_mutex_unlock(&arg->mutex);
    return NULL;
}

// thread p main function
void *thread_p(void *arg_in) {
    thread_arg_t *arg = arg_in;
    int min = -1;
    int max;
    int guess;
    int result;

    my_msleep(arg->seed);

    // lock the shared structure
    pthread_mutex_lock(&arg->mutex);

    // get max value from thread_c
    while(arg->status != S_MAX) {
        pthread_cond_wait(&arg->cond_resilt, &arg_mutex);
    }
    max = arg->max;
    do {
        guess = (min + max)/2;
        printf("My guess: %d\n", guess);

        // send guess to thread_c
        arg->guess = guess;
        arg->status - S_GUESS;
        pthread_cond_signal(&arg->cond_guess); // wakes up thread c

        // waits for result from thread c
        while(arg->status != S_RESULT) {
            pthread_cond_wait(&arg->cond_result, &arg->mutex);
        }
        
        // copy arg->result to local result
        result = arg->result;
        if(result > 0) {
            min = guess + 1;
        } else if(result < 0) {
            max = guess - 1;
        } 
    } while(result != 0);

    // unlock thread before exiting and printing
    pthread_mutex_unlock(&arg->mutex);

    printf("%s", arg->message); // arg->message not changed after it is set

    return NULL;
}

int main(int argc, char *argv[]) {
    // starter code parsing logic here

    if(demo) {
        guess_my_number(seed);
        exit(0);
    }

    thread_arg_t arg;
    arg.seed = seed;

    // initializing synch primitives
    pthread_mutex_init(&arg.mutex, NULL);
    pthread_cind_init(&arg.cond_guess, NULL);
    arg.status = S_INIT;

    pthread_t tc, tp; // creates the IDs for threads c and p

    //creating two threads
    if(pthread_create(&tc, NULL, thread_c, &arg) != 0) {
        die("Failed to create thread C");
    }
    if(pthread_create(&tp, NULL, thread_p, &arg) != 0) {
        die("Failed to create thread P");
    }

    // wait for t and c to finish
    pthread_join(tc, NULL);
    pthread_join(tp, NULL);

    // clean OS resources
    pthread_mutex_destroy(&arg.mutex);
    pthread_cond_destroy(&arg.cond_guess);
    pthread_cond_destroy(&arg.cond_result);

    return 0;
    }
```
## Going Deeper
```C
// The States
enum {S_INIT, S_GUESS, S_RESULT, S_MAX};

typedef struct thread_arg_tag {
    // ... data fields (seed, max, guess, result) ...
    int status;                     // The current state of the game
    pthread_mutex_t mutex;          // The lock preventing simultaneous access
    pthread_cond_t cond_guess;      // Wake-up bell for Thread C (guess is ready)
    pthread_cond_t cond_result;     // Wake-up bell for Thread P (max/result is ready)
} thread_arg_t;
```
- threads cannot just read arg->guess whenever
  - need to check arg->status for their turn, and will go to sleep if it is not with a *condition variable*
    - cond_guess or cond_result
- pitfall: data race
  - removing the mutex causes both threads to read arg->status *at the same time*
    - cannot be controlled, and leads to unpredictable behavior
  - program needs the mutex to regulate who gets to the critical section at what point (one thread "holds the key")
```C
// 1. Initialize the synchronization primitives
    // NULL means we are using default attributes.
    pthread_mutex_init(&arg.mutex, NULL);
    pthread_cond_init(&arg.cond_guess, NULL);
    pthread_cond_init(&arg.cond_result, NULL);
    
    // Set the initial traffic light
    arg.status = S_INIT;

    pthread_t tc, tp;

    // 2. Spawn the threads
    if (pthread_create(&tc, NULL, thread_c, &arg) != 0) die("Failed");
    if (pthread_create(&tp, NULL, thread_p, &arg) != 0) die("Failed");

    // 3. Wait for the game to end
    pthread_join(tc, NULL);
    pthread_join(tp, NULL);

    // 4. Clean up OS resources
    pthread_mutex_destroy(&arg.mutex);
    pthread_cond_destroy(&arg.cond_guess);
    pthread_cond_destroy(&arg.cond_result);
```
- pthread_cond_init and pthread_mutex_init
  - asks operating system to allocate internal tracking structs needed to lock and signal for the critical section
- pitfall: resource leaks
  - removing pthread_join causes main thread to hit zero first
    - kills the proccess before the child ones finish playing
  - removing pthread_mutex_destory and pthread_cond_destroy leads to orphaned locks and memory --> *excess burden*
```C
void * thread_c(void * arg_in) {
    thread_arg_t *arg = arg_in;
    gmn_t gmn;
    gmn_init(&gmn, arg->seed);
    my_msleep(rand());

    // LOCK: Grab the key before touching the shared 'arg' struct
    pthread_mutex_lock(&arg->mutex);

    // Turn 1: Give Thread P the max value
    arg->max = gmn_get_max();
    arg->status = S_MAX;                     // Change the traffic light
    pthread_cond_signal(&arg->cond_result);  // Ring the bell to wake up Thread P

    int result = -1;
    while (result != 0) {
        // WAIT LOOP: Sleep until it is exactly S_GUESS
        while (arg->status != S_GUESS) {
            // This function unlocks the mutex, goes to sleep, and re-locks 
            // the mutex the exact millisecond it wakes up.
            pthread_cond_wait(&arg->cond_guess, &arg->mutex);
        }

        // Process the guess
        result = gmn_check(&gmn, arg->guess);
        arg->result = result;
        if (result == 0) strcpy(arg->message, gmn_get_message(&gmn));

        // Turn Complete: Hand control back to P
        arg->status = S_RESULT;                  
        pthread_cond_signal(&arg->cond_result);  
    }
    
    // UNLOCK: Release the key so Thread P can finish up
    pthread_mutex_unlock(&arg->mutex);
    return NULL;
}
```
- pthread_cond_wait is the most *complex* pthread function in the library
  - the OS puts thread c to sleep when called, but also forces it to *drop* its key to the critical space
    - if not dropped, thread p could not gain access to the critical space to submit the guess
  - thread c is signaled to wake up later, and then waits until the mutex is free and resumes execution
- pitfall: spurious wakeups
  - OS scheduler can wake up a thread from pthread_cond_wait **without** a signal sometimes
    - if(arg->status != S_GUESS) would lead to a spurious wakup leading to a garbage guess
      - **always use a while loop for condition waits** to avoid the issue
```C
void * thread_p(void *arg_in) {
    thread_arg_t *arg = arg_in;
    int min = 1, max, guess, result;
    my_msleep(arg->seed);

    // LOCK: Grab the key
    pthread_mutex_lock(&arg->mutex);

    // Wait for the game to start (Wait for S_MAX)
    while (arg->status != S_MAX) {
        pthread_cond_wait(&arg->cond_result, &arg->mutex);
    }
    max = arg->max;

    do { 
        // Calculate the next binary search target
        guess = (min + max)/2;
        printf("My guess: %d\n", guess);

        // Turn: Submit the guess
        arg->guess = guess;
        arg->status = S_GUESS;
        pthread_cond_signal(&arg->cond_guess); // Ring bell for Thread C

        // WAIT LOOP: Sleep until Thread C grades the guess
        while (arg->status != S_RESULT) {
            pthread_cond_wait(&arg->cond_result, &arg->mutex);
        }
        
        result = arg->result; // Read the grade safely while locked

        // Adjust bounds for the next loop
        if (result > 0) min = guess + 1;
        else if (result < 0) max = guess - 1;
            
    } while (result != 0);

    // UNLOCK: The game is over, drop the key.
    pthread_mutex_unlock(&arg->mutex);

    printf("%s", arg->message);
    return NULL;
}
```
- thread p is the "consumer" to thread c, which is the "producer"
  - calculates, pushes data to struct, signals condition, and goes back to sleep while waiting for response 
- pitfall: deadlock via missed signals 
  - if thread p didn't lock mutex before checking arg->status...
    - thread p checks status, sees its not S_RESULT, and gets ready to sleep
    - **AT THE SAME TIME**, thread c updates status to S_RESULT and fires pthread_cond_signal
  - leads to thread p executing pthread_cond_wait and sleeping forever, since the signal to wake up already fired
  - **HOLD MUTEX LOCK BEFORE WAITING ON CONDITION**