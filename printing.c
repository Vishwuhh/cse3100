#define _BSD_SOURCE
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

// error handling functions

void die(char *s)
{
    if (errno)
        perror(s);
    else 
        fprintf(stderr, "Error: %s\n", s);
    exit(EXIT_FAILURE);
}

void check_pthread_return(int rv, char *msg)
{
    if (rv == 0)
        return;
    errno = rv;
    die(msg);
}

#define DEFAULT_NUM_JOBS    20
#define DEFAULT_NUM_PRINTERS 2

#define print_job(x) usleep((x)*100 + 50)

typedef struct {
    unsigned char *jobs;            // buffer that keeps job info
    int     njobs_fetched;          // number of jobs that have fetched by printer
    int     njobs_max;              // number of jobs to perform
    pthread_mutex_t mutex;          // mutex to protect the queue, which is shared by all threads
} job_queue_t;

// The computers and printers can share the same structure
typedef struct {
    int id;
    job_queue_t * jq;
    int njobs;   // number of jobs printed
    pthread_t thread_id; // thread id
} printer_t;

/************* BEIGINNING OF QUEUE *********************/
/* Impelementation of Q. Not a small, fixed-size buffer, but good enough for this assignment. */

// init q, add random max_jobs jobs, seed is set before this function
int q_init(job_queue_t *q, int max_jobs)
{
    q->njobs_fetched = 0;
    q->njobs_max = max_jobs;
    q->jobs = malloc(max_jobs);
    if (q->jobs == NULL)
        die("malloc()");
    for (int i = 0; i < max_jobs; i ++)
        q->jobs[i] = rand() % 100 + 1;
    return 0;
}

// get the number of jobs to be printed
// Return values:
//      non-negative integer:  the number of remaining jobs in the queue
int q_num_jobs(job_queue_t *q)
{
    return (q->njobs_max - q->njobs_fetched);