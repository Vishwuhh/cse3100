# Pitfall
- would anything go wrong?
```C
pid_t pid = fork();
if (pid < 0) {
    perror("fork()"); exit(1);   // exit if fork() fails
} else if (pid == 0) {
    child_tasks();              // Child executes this
} else {
    parent_tasks();             // Parent executes this
}
more_parent_tasks();
```
## The Issue
- there is no exit command inside the *else if(pid == 0)* block
  - leads to the child process continuing after finishing child_tasks() and then would run more_parent_tasks()
    - bad because **this is only for the parent proccess**
## The solution
```C
else if (pid == 0) {
    child_tasks();
    exit(0); // This terminates the child process immediately
}
```
- exit(0) would help terminate the child proccess after it finishes its task
  - prevents *proccess pollution*

# Proccess Upgrades
- a fresh clone would want to run a *different code*
- done through loading another executable in the proccess address space
  - NOTE: picked up from the file system
- **OPENED FILES ARE NOT AFFECTED BY UPGRADE OPERATION**

# The *exec* Family
- act of upgrading is done by the child within a system call
  - "man -S3 execl" to get more details 
    ```C
    #include <unistd.h>
    int execl(const char *path, const char *arg0, ..., /*, (char *) NULL */);
    ```
    - path to the executable to load isnide our address space
    - list of arguments to be passsed to the *new executable*
    - **final NULL pointer* to show the end of arg list
    - **WHEN SUCCESSFUL**, execl() does not return! it starts a new proccess
## *exec* Example
- "adder" program
    ```C
    #include <stdio.h>
    #include <stdlib.h>

    int main(int argc, char* argv[]) {
        int i, sum = 0;
        for(i = 1; i < argc; i++)
            sum += atoi(argv[i]); // Convert string argument to integer [cite: 58]
        printf("sum is: %d\n", sum);
        return 0;
    }
    ```
- parent program
    ```C
    #include <stdio.h>
    #include <unistd.h> // Required for fork and execl [cite: 42, 149]

    int main() {
        char *cmd1 = "./adder", *cmd2 = "expr";
        pid_t child = fork(); // Create a new process [cite: 67]

        if (child == 0) {
            // --- CHILD PROCESS ---
            printf("In child!\n");
            // Replaces the child process with "./adder" [cite: 70]
            // Arguments: path, argv[0], "1", "2", "3", "10", NULL terminator [cite: 70]
            execl(cmd1, cmd1, "1", "2", "3", "10", NULL); 

            // If execl is successful, the following lines are NEVER reached [cite: 48]
            printf("Oops.... something went really wrong!\n");
            perror(cmd1);
            return -1;
        } else {
            // --- PARENT PROCESS ---
            printf("In parent!\n");
            // Replaces the parent process with the system utility "expr" [cite: 76]
            execl(cmd2, cmd2, "100", "+", "300", NULL);

            // If execl is successful, the following lines are NEVER reached [cite: 48]
            printf("Oops.... something went really wrong!\n");
            perror(cmd2);
            return -1;
        }
    }
    ```
- reminders
  - NULL terminator
    - every execl call needs to end with a (char *) NULL to signal the end of the arugment list
  - no return on success
    - the original program is gone from the process's memory if execl works, so returns only when there is error
  - pathing
    - ./adder needs the program to be in the *current* directory, execlp ('p' variant) could be used to search PATH automaticaly

# How is the Executable Found?
- specify a path such as */bin/ls*
- specify a file, and system searches on directories listen in PATH
  - echo $PATH in bash to see directories seperated by ':'
```C
int excel(const char *path, const char *arg0, ..., 
    /*, (char *) NULL */);

// execlp() searches path for target file 
int excel(const char *file, const char *arg0, ..., 
    /*, (char *) NULL */);
```

# *execv* Family
```C
// if number of args is unknown at compile time 
#include <unistd.h>
int execv(const char *path, char *const argv[]);
int execvp(const char *file, char *const argv[]);
```
- arguments in execl are placed **in an array**
  - argv is the argv in the main function
- execv needs a path, but the execvp searches file in PATH
- 




