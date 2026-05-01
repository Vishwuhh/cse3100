# Problem 1: Coins (synchronization)
- solution
  - tosser must wait on cond_toss if data.ready == 1
    - previous toss hasn't been read yet
  - once a new value is made, it signals a conditon variable cond[c] for the corresponding counter 
    - when all tosses are made (when n == 0), it needs to wake up all the waiting counters so they can exit 
  - counters wait on their unique cond[id] until either a matching value is made **or** the tossing is complete
    - once value is consumed, they set ready = 0 and signals tosser to resume 
## tosser function
```C
void *tosser(void* t) {
    thread_arg_t *arg = t;
    int id = arg->id;
    data_t *pdata = arg->pdata;
    unsigned s = id;
    int done = 0; 

    while(!done) {
        pthread_mutex_lock(&pdata->mutex); // locks mutex before accessing shared data
        // waits until prev toss has been processed by counter
        while(pdata->ready) {
            pthread_cond_wait(&pdata->cond_toss, &pdata->mutex); // &pdata->ready = grab mutex var inside pdata, and get the memory address of the specific mutex
        }
        if(pdata->n == 0) {
            done = 1; // variable signals the tossing is over
            // wake up all counter to evaluate end condition
            for(int i = 0; i < NUM_COUNTERS; i++) {
                pthread_cond_signal(&pdata->cond[i])
            }
        } else {
            // generates the new coin toss values
            int c1 = rand_r(&s) & 1;
            int c2 = rand_r(&s) & 1;
            int c3 = rand_r(&s) & 1;
            int c4 = rand_r(&s) & 1;
            int c = (c1 << 3) | (c2 << 2) | (c3 << 1) | c4;
            
            pdata->data = c; // publishes new cointoss
            pdata->ready = 1; // signals to the counters that there is new data
            pdata->n = pdata->n - 1; // total number of tosses left

            pthread_cond_signal(&pdata->cond[c]); // wakes up speciifc counter for the value 
        }
        // unlocks mutex
        pthread_mutex_unlock(&pdata->mutex);
    }
    pthread_exit(NULL);
}
```
## counter function - corrected
```C
void *counter(void *t) {
  thread_arg_t *arg = t;
  int id = arg->id;
  int *p_count = arg->p_count;
  data_t *pdata = arg->pdata;

  int done = 0;
  while (!done) {
    // 1. Lock the mutex
    pthread_mutex_lock(&pdata->mutex);

    // 2. Wait until there is data meant for THIS specific thread, OR the tosser is completely finished
    while (!(pdata->ready && pdata->data == id) && !(pdata->n == 0 && !pdata->ready)) {
        pthread_cond_wait(&pdata->cond[id], &pdata->mutex);
    }

    if (pdata->ready && pdata->data == id) { // if val of both ready and data vars are equal to the ID
      (*p_count)++; // increment the integer in the memory address of p_count
      pdata->ready = 0; // sets value of var ready in pdata to zero
      // Signal the tosser that the data has been consumed and it can generate the next
      pthread_cond_signal(&pdata->cond_toss); // signals thread with that is waiting on the specifc cond_toss conditonal variable
    }

    if (!pdata->ready && pdata->n == 0) {
      done = 1; // Exit the loop
    }

    // 3. Unlock the mutex
    pthread_mutex_unlock(&pdata->mutex);
  }
  pthread_exit(NULL);
}
```
# Problem 2 (monte carlo multithreading)
```C
#include <ctype.h>
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define N_THREADS 2
#define SAMPLE_SIZE 10000000

// The private workspace for each thread
typedef struct thread_arg_tag {
  int id;
  int count;
  int total;
} thread_arg_t;

double rand_double(unsigned int *s) { return (rand_r(s) / (double)RAND_MAX); }

// Worker Thread: Executes the Monte Carlo simulation
void *thread_main(void *thread_arg) {
  // 1. The Void Pointer Cast: Translate the generic pointer back into our struct pointer
  thread_arg_t *arg = (thread_arg_t *)thread_arg;
  
  // 2. Thread-Safe Randomness: Use the thread ID to seed rand_r safely
  unsigned int seed = arg->id;
  int count = 0;
  double x, y;

  // 3. The Workload: Only run the simulation for this thread's assigned chunk
  for (int i = 0; i < arg->total; i++) {
    x = rand_double(&seed);
    y = rand_double(&seed);
    
    // Check if point is inside the unit circle (x^2 + y^2 <= 1)
    if (x * x + y * y <= 1.0) {
      count++;
    }
  }
  
  // 4. Save the result. No mutex is needed because this thread is writing to 
  //    its own isolated index in the thread_args array.
  arg->count = count;
  
  pthread_exit(NULL);
}

int main(int argc, char *argv[]) {

  int n = SAMPLE_SIZE;
  int n_threads = N_THREADS;

  int total_count = 0;

  if (argc > 1) {
    n = atoi(argv[1]);
  }
  if (argc > 2) {
    n_threads = atoi(argv[2]);
  }

  pthread_t thread_arr[n_threads];
  thread_arg_t thread_args[n_threads];

  // 1. Calculate the workload distribution
  // e.g., 10,000,000 / 3 gives a base of 3,333,333 and a remainder of 1
  int base_chunk = n / n_threads;
  int remainder = n % n_threads;

  // 2. Initialize structs and spawn the threads
  for (int i = 0; i < n_threads; i++) {
    thread_args[i].id = i;
    thread_args[i].count = 0;
    
    // Distribute the remainder points evenly among the first few threads using a ternary operator
    thread_args[i].total = base_chunk + (i < remainder ? 1 : 0);
    
    // Pass the specific memory address of this thread's struct
    pthread_create(&thread_arr[i], NULL, thread_main, &thread_args[i]);
  }

  // 3. The Barrier: Wait for threads to finish and sum their individual counts
  for (int i = 0; i < n_threads; i++) {
    // pthread_join forces main to wait until thread_arr[i] terminates completely
    pthread_join(thread_arr[i], NULL);
    
    // Safe Harvesting: Because we joined, we know this count is final
    total_count += thread_args[i].count;
  }

  // Calculate and print the final Pi approximation
  double pi = 4.0 * total_count / (double)n;

  printf("Sample size: %d\n", n);
  printf("Number of threads: %d\n", n_threads);
  printf("pi = %.2f\n", pi);

  return 0;
}
```