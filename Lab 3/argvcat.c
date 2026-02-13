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
    size_t len1 = strlen(s1);
    size_t len2 = strlen(s2);

    char *ret = malloc(len1 + len2 + 1);

    if(ret == NULL){
        my_error("malloc failed");
    }

    strcpy(ret, s1);
    strcat(ret, s2);

    return ret;
}

int main(int argc, char *argv[])
{
    char    *s;

    s = my_strcat("", argv[0]);

    for (int i = 1; i < argc; i ++) {
        char *temp = my_strcat(s, argv[i]);
        free(s);
        s = temp;
    }

    printf("%s\n", s);

    return 0;
}
