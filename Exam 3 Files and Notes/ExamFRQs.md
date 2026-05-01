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