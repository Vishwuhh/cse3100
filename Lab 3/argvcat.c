#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* print out an error message and exit */
void my_error(char *s)
{
    perror(s);
    exit(1);
}

/* Concatnate two strings.
 * Dynamically allocate space for the result.
 * Return the address of the result.
 */
char *my_strcat(char *s1, char *s2)
{
    // calculates length of two strings by counting the visible characters (IGNORES the NULL terminator)
    // need to add +1 manually or it would write the NULL terminator into unallocated memory, causing a Segmentation Error
    size_t len1 = strlen(s1);
    size_t len2 = strlen(s2);
    // need to allocate the exact byte count from the heap with malloc(), which helps the memory survive post function run
    char *ret = malloc(len1 + len2 + 1);

    if(ret == NULL){ // errors out of there is no more memory left to allocate
        my_error("malloc failed");
    }
    // strcpy() makes the base string by copying what is in the s1 string into the empty ret block
    strcpy(ret, s1);
    // strcat() goes to the end of ret block and find null terminator added by strcpy()
    // overwrites the null terminator with the first char of s2, and appends the rest of s2 onto s1 with a new terminator
    strcat(ret, s2);

    return ret;
}

int main(int argc, char *argv[])
{
    char    *s;
    // initializes tracking pointer with the first command-line argument (program name)
    s = my_strcat("", argv[0]);

    // loop is for safe pointer swapping, since my_strcat dynamically allocates NEW blocks of memory and holds its address in a temporary pointer
    for (int i = 1; i < argc; i ++) {
        char *temp = my_strcat(s, argv[i]);
        free(s); // need to free the memory the old block that 's' was pointing too or else it would cause a leak
        s = temp;
    }

    printf("%s\n", s);
    free(s); // fress the large concatenated string that is still in the heap
    return 0;
}
