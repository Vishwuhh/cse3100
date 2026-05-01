#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_CHAIRS = 2

typedef struct{
    unsigned int id;
} thread_arg_s;

typedef struct{
    int id;
    int max;
    int chair;
    int talked;
    int talking; 
    int room;
} thread_arg_p;

void *Student(void * t) {
    thread_arg_s *arg = t;
    int id;
    unsigned s = id;
    int arrived = 0;
}

void *Professor(void *t) {
    thread_arg_p *arg = t;
    unsigned int max = arg->max;
    int chair[2]; 
    unsigned int arrived = 0;
    int talked = 0; 

    while(arg->room < 2 && arg->talked < arg->max) {
        arrived = arrived + 1; 
        printf("Student %d knocks the door", arrived);     
        chair[arg->room] = arrived; 
        printf("Student %d is waiting in the chair", arrived); 
        arg->room++;
        while (arg->talking == 1) {
            talked += 1; 
            printf("Professor is talking to student %d, consultation %d", arrived, talked); 
        }
    }
}

int main(int argc, char *argv[], int n, int p, int s) {
    if(argc != 2) {
    printf("Usage: %s n\n", argv[0]);
    exit(0);
    }

    srand(s); 

    int chair[2]; 

    printf("Enter number of students:\n");
    scanf("%d", &n);
    printf("Enter max conversations with students:\n");
    scanf("%d", &p);

    printf("%d is the seed\n", s);
    printf("%d is the maximum muber of students in the simulation (n)\n", n);
    printf("%d is the maximum number of students the professor will talk to (p)\n", p); 

    thread_arg_s threads[n];

    for(int i = 0; i < n; i++) {
        threads[i].id = i;
        
    }
}



