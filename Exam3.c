#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void task() {
    char *str = malloc(100);
    snprintf(
        str, 
        100,
        "No ampount of money every bough a second of time !!"
    );
    pthread_exit(str);
}

void *thread() {
    int *c = malloc(4);
    task();
    *c = 420;
    return c;
}