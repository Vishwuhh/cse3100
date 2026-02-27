/*  In this assignment, we experiment with passing references to functions

    On the following web site, you can find a swap funciton that swaps two integers.
    https://www.tutorialspoint.com/cprogramming/c_function_call_by_reference.htm

    In this exercise, we implement two functions. 
    One function swaps two pointers to int,  and 
    the other swaps structures. 

    Search TODO to find the locations where we need to work 
*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>

#define     NAME_SIZE    64

typedef struct node_tag
{
    unsigned int    id;   
    char   name[NAME_SIZE];
} person_t;
// int** is used since we are swapping addresses stored inside the int* variables
void swap_pointer_int(int **p1, int **p2) { 
    // temp is an int* since it holds the memory addresses we are swapping
    int *temp = *p1; // *p1 dereferences the pointer to grab the actual pointer from caller
    *p1 = *p2;
    *p2 = temp;
}

void test_swap_pointer_int(int argc, char *argv[])
{
    int     a = 10, b = 20;
    int     *pa = &a, *pb = &b;

    if (argc >= 2) 
        a = atoi(argv[1]);

    if (argc >= 3) 
        b = atoi(argv[2]);

    printf("Before swap\n");
    printf("*pa is %d, *pb is %d\n", *pa, *pb);

    swap_pointer_int(&pa, &pb); // need to pass the address of pa and pb using & operator since it expects int** addreses

    printf("After swap\n");
    printf("*pa is %d, *pb is %d\n", *pa, *pb);
}

void swap_person(person_t *p1, person_t *p2) { // using persion_t* to point to the original structures in memory
    // *p1 dereferences the pointer and grabs the whole structure block, and is then saved into a struct variable
    person_t temp = *p1;
    // overwrites the first structure block with the second strucutre block
    *p1 = *p2;
    // puts the saved block into the second structure
    *p2 = temp;
};

void test_swap_person(int argc, char *argv[])
{
    // initialize structures  
    person_t   x = {1, "Alice"}, y = {2, "Bob"};

    if (argc >= 2) {
        strncpy(x.name, argv[1], NAME_SIZE - 1);
        x.name[NAME_SIZE - 1] = 0;
    }

    if (argc >= 3) {
        strncpy(y.name, argv[2], NAME_SIZE - 1);
        y.name[NAME_SIZE - 1] = 0;
    }

    printf("Before swap\n");
    printf("x's id is %d, x's name is %s\n", x.id, x.name);
    printf("y's id is %d, y's name is %s\n", y.id, y.name);

    swap_person(&x, &y); // passes the addresses of the structures using the & operator
    
    printf("After swap\n");
    printf("x's id is %d, x's name is %s\n", x.id, x.name);
    printf("y's id is %d, y's name is %s\n", y.id, y.name);
}

/* Do not change main() */
int main(int argc, char *argv[])
{
    if (argc == 1) {
        test_swap_pointer_int(argc, argv);
        test_swap_person(argc, argv);
    }
    else if (isdigit(argv[1][0]))  // if the first argument starts with a digit
        test_swap_pointer_int(argc, argv);
    else 
        test_swap_person(argc, argv);
    return 0;
}
