## Lab 7 Review
- solving madd.c 
```C
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "matrix.h" // the matrix.h file given to us

#define NUM_THREADS 2 // number of available threads is two

typedef struct{
    unsigned int id;
    TMatrix *m, *n, *t;
} thread_arg_t; 

// main function of threads
static void* thread_main(void* p_arg){
    // unpack the arguments
    // need to cast generic void pointer back to the specific struct type we defined to access the data

    thread_arg_t *arg = (thread_arg_t *)p_arg;

    // detemine the amount of workload for the thread based on its ID
    // split the rows based on the number of threads

    unsigned int start_row = arg->id*(arg->m->nrows / NUM_THREADS);  // calculates the starting point
    // hands odd number of rows for matrices
    if(arg->id == NUM_THREADS - 1) {
        end_row = arg->m->nrows; // end boundary is the total number of rows 
    } else {
        end_row = (arg->id + 1)*(arg->m->nrows / NUM_THREADS); // normal boundary calculation
    }

    // perform matrix addition for assigned rows 
    for(unsigned int i = start_row; i < end_row; i++) {
        for(unsigned int j = 0; j < arg->m->ncols; j++) {
            // adds elements together and stores result in matrix 't'
            arg->t->data[i][j] = arg->m->data[i][j] + arg->n->data[i][j]
        }
    }
}

 // return the sum of two matrices, which is the newly created matrix
    // pthread function will report error if failed, and NULL if something else is wrong
    // like addMatrix, but uses two threads

    TMatrix *addMatrix_thread(TMatrix *m, TMatrix *n) {
        // input validation, from lab template
        if(m == NULL || n == NULL || m->nrows != n->nrows || m->ncols != n->ncols) {
            return NULL;
        }
        // allocate result matrix
        TMatrix *t = newMatrix(m->nrows, m->ncols);
        if(t == NULL) {
            return t; 
        }
        // declare arrays for thread IDs and thread args
        pthread_t threads[NUM_THREADS];
        thread_arg_t args[NUM_THREADS];

        // spawn threads
        for(unsigned int i = 0; i < NUM_THREADS; i++) {
            // populate arg struct for the specific thread
            args[i].id = i;
            args[i].m = m;
            args[i].n = n;
            args[i].t = t;

            // create thread, passing addr of the corresponding argument struct
            // check for non-zero return value to handle errors
            if(pthread_create(&threads[i], NULL, thread_main, (void*)&args[i]) != 0) {
                fprintf(stderr, "Error: Failed to create thread %u\n", i);
                exit(EXIT_FAILURE);
            }
        }

        // wait for all the threads to finish, and join them
        // need to do so in a SEPERATE loop after all threads made, else the program will run sequentially
        for(insigned int i = 0; i < NUM_THREADS; i++) {
            if(pthread_join(threads[i], NULL) != 0) {
                fprintf(stderr, "Error: Failed to join thread%u\n", i);
                exit(EXIT_FAILURE);
            }
        }
    }
// once done, resulting matrix 't' is fully populated
return t; 
```
## Deep Dive
```C
#define NUM_THREADS 2

typedef struct {
    unsigned int id;
    TMatrix *m, *n, *t;
} thread_arg_t;
```
- spawining threads with pthread_create only allows passing a single generic memory pointer (void *) to thread
  - adding two matrices and storing the result requires the thread to know *four things*: id, two input matrices (m, n), and output matrix (t)
- packing into single struct allows us to pass a *single pointer* to the thread that has access to everything 
```C
static void * thread_main(void * p_arg)
{
    // 1. Memory Re-casting
    thread_arg_t *arg = (thread_arg_t *)p_arg;

    // 2. Data Partitioning (Process Coordination)
    unsigned int start_row = arg->id * (arg->m->nrows / NUM_THREADS);
    unsigned int end_row;

    if (arg->id == NUM_THREADS - 1) {
        end_row = arg->m->nrows; 
    } else {
        end_row = (arg->id + 1) * (arg->m->nrows / NUM_THREADS);
    }
    // 3. The Computation
    for (unsigned int i = start_row; i < end_row; i++) {
        for (unsigned int j = 0; j < arg->m->ncols; j++) {
            arg->t->data[i][j] = arg->m->data[i][j] + arg->n->data[i][j];
        }
    }
    return NULL;
}
```
- memory recasting
  - system passed out structure as a generic pointer void*, so we need to cast it *back* into thread_arg_t*
    - compiler needs to know how to read memory layout
- data paritioning
  - need to divide the total workload by the number of threads
    - thread 0 gets top half, thread 1 gets bottom half
  - if(arg->id == NUM_THREADS - 1) checks if the matrix has an odd number of rows
    - if so, final thread catches the remainder so all memroy is allocated
- computation
  - for-loop matrix addition, bounded by the start and end row assigned for the thread
```C
TMatrix * addMatrix_thread(TMatrix *m, TMatrix *n)
{
    // 1. Input Validation
    if (    m == NULL || n == NULL
         || m->nrows != n->nrows || m->ncols != n->ncols )
        return NULL;
    // 2. Memory Allocation
    TMatrix * t = newMatrix(m->nrows, m->ncols);
    if (t == NULL)
        return t;
    // 3. Thread Preparation
    pthread_t threads[NUM_THREADS];
    thread_arg_t args[NUM_THREADS];
    // 4. Thread Creation (Spawning)
    for (unsigned int i = 0; i < NUM_THREADS; i++) {
        args[i].id = i;
        args[i].m = m;
        args[i].n = n;
        args[i].t = t;

        if (pthread_create(&threads[i], NULL, thread_main, (void *)&args[i]) != 0) {
            fprintf(stderr, "Error: Failed to create thread %u\n", i);
            exit(EXIT_FAILURE);
        }
    }
    // 5. Thread Synchronization (Joining)
    for (unsigned int i = 0; i < NUM_THREADS; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            fprintf(stderr, "Error: Failed to join thread %u\n", i);
            exit(EXIT_FAILURE);
        }
    }
    // 6. Return Result
    return t;
}
```
- input validation and allocation
  - function checks for NULL values and dimension mismatches
    - if true, returns NULL to indicate something is wrong
    - if false, allocates heap memory for the results matrix 't' with newMatrix() API
- thread preparation
  - declare the arrays for both thread IDs and args
    - seperate arrays ensures each thread is *isolated* and has dedicated space for their inputs
- creation and sychronization
  - multithreading proccess
  - first loop signals all threads to do work *asynchronously*
  - second loop uses pthread_join() to force main exec thread to pause until 0 and 1 is done with their main functions 
    - if not joined, function would return matrix t **before** worker threads put numbers inside fo it