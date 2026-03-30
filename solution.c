#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    // pipes
    int p_to_c1[2];
    int c1_to_c2[2];
    int c2_to_p[2];

    if(pipe(p_to_c1) == -1) {
        exit(EXIT_FAILURE);
    } else if(pipe(c1_to_c2) == -1) {
        exit(EXIT_FAILURE);
    } else if(pipe(c2_to_p) == -1) {
        exit(EXIT_FAILURE);
    }
    // child 1
    pid_t pid1 = fork();
    if(pid1 < 0) {
        exit(EXIT_FAILURE);
    }

    if(pid1 == 0) {
        // does not use C2 to P pipe
        close(c2_to_p[1]);
        close(c2_to_p[0]);

        close(c1_to_c2[0]); // writes from c1 to c2 only
        close(p_to_c1[1]); // reads from parent to c1 only
        
        int i = 0;
        char str;

        if(read(p_to_c1[0], &str, sizeof(char)) > 0) {
            char *str;
            size_t len = (*str != NULL) ? strlen(*str) : 0;
            for(i = 0; str[i] != str[0]; i < len) { // removes the char at index 0
            str[i] = str[i+1]; 
            }
            printf(str);
            write(c1_to_c2[1], &str, sizeof(char));
        }
        // closes used FDs
        close(p_to_c1[0]);
        close(c1_to_c2[1]);
        
    }
    // child 2
    pid_t pid2 = fork();
    if(pid2 < 0) {
        exit(EXIT_FAILURE);
    }
    if(pid2 == 0) {
        close(p_to_c1[1]);
        close(p_to_c1[0]);

        close(c1_to_c2[1]); // reads from c1 only
        close(c2_to_p[0]); // writes to parent only

        int i = 0;
        char str; 

        if(read(c1_to_c2[0], &str, sizeof(char)) > 0) {
            char *str;
            size_t len = (*str != NULL) ? strlen(*str) : 0;
            for(i = 0; str[i] != str[0]; i < len) { // removes the char at index 0
            str[i] = str[i+1]; 
            }
            printf(str);
            write(c2_to_p[1], &str, sizeof(char));
        }
    }
    // does not use child connecting pipe
    close(c1_to_c2[0]);
    close(c1_to_c2[1]);

    close(p_to_c1[0]); // writes to c1, doesnt read
    close(c2_to_p[1]); // reads from c2, doesnt write

    char str[0] = '\0';
    int i = 0;
    printf(str);
    write(p_to_c1[1], &str, sizeof(char));
    
    if(read(c2_to_p[0], &str, sizeof(char)) > 0) {
        char *str;
        size_t len = (*str != NULL) ? strlen(*str) : 0;
        for(i = 0; str[i] != str[0]; i < len) { // removes the char at index 0
        str[i] = str[i+1]; 
        }
        printf(str);
         write(p_to_c1[1], &str, sizeof(char));
    }
    // closed used pipes
    close(c2_to_p[0]);
    close(p_to_c1[1]);
    // wait for children
    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);
    return 0;
}