#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "matrix.h"

#define NUM_THREADS 2

typedef struct {
    unsigned int id;
    TMatrix *m, *n, *t;
} thread_arg_t;

static void * thread_main(void * p_arg)
{
    thread_arg_t *arg = (thread_arg_t *)p_arg;
    unsigned int id = arg->id;
    TMatrix *m = arg->m;
    TMatrix *n = arg->n;
    TMatrix *t = arg->t;

    // Divide the rows among the threads
    unsigned int rows_per_thread = m->nrows / NUM_THREADS;
    unsigned int start_row = id * rows_per_thread;
    
    // Ensure the last thread gets any remaining rows if nrows isn't perfectly divisible
    unsigned int end_row = (id == NUM_THREADS - 1) ? m->nrows : start_row + rows_per_thread;

    for (unsigned int i = start_row; i < end_row; i++) {
        for (unsigned int j = 0; j < n->ncols; j++) {
            TElement sum = (TElement)0;
            for (unsigned int k = 0; k < m->ncols; k++) {
                sum += m->data[i][k] * n->data[k][j];
            }
            t->data[i][j] = sum;
        }
    }
    return NULL;
}

TMatrix * mulMatrix_thread(TMatrix *m, TMatrix *n)
{
    if (m == NULL || n == NULL || m->ncols != n->nrows)
        return NULL;

    TMatrix * t = newMatrix(m->nrows, n->ncols);
    if (t == NULL)
        return t;

    pthread_t threads[NUM_THREADS];
    thread_arg_t args[NUM_THREADS];

    // Create threads
    for (unsigned int i = 0; i < NUM_THREADS; i++) {
        args[i].id = i;
        args[i].m = m;
        args[i].n = n;
        args[i].t = t;
        if (pthread_create(&threads[i], NULL, thread_main, &args[i]) != 0) {
            perror("Failed to create thread");
            exit(EXIT_FAILURE);
        }
    }

    // Wait for threads to finish
    for (unsigned int i = 0; i < NUM_THREADS; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            perror("Failed to join thread");
            exit(EXIT_FAILURE);
        }
    }

    return t;
}