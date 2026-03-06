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
- starts a new peocess if successful, and is similar to execl

# *argv* to *execv* and *execvp*
- argv goes into a stack with the *last* pointer being NULL
```C
  $gcc -o ab ab.c
```
- "$gcc" = arg[0]
- "-o" = arg[1]
- "ab" = arg[2]
- "ab.c" = arg[3]

# Question
- what could go wrong?
```C
pid_t pid = fork();
if(pid < 0) {
    perror("fork()"); exit(1); // the exit() is for if fork() fails 
}
else if(pid == 0) {
    execlp("genie", "genie", "clean the house", NULL); // child proccess
}
online_shopping(); // parent proccess
```
- when genie finishes cleaning, it doesnt disappear and instead becomes a **zombie**
  - stays in the system so the parent can read its exit status 
- if the program genie is missing instead of executable, execlp returns to the **child process**
  - since there is no exit() or error handling after the execlp call, child process will *continue executing the rest of the code*
    - would have two processes both buying something, which is not what we want
- if the parent finishes online_shopping() and exits *before* child process finishes genie, the child becomes an **orphan**
  - parent dies and the child is re-paranted to a subreaper like init or systemd
  - if the child expected to communicate with the parent or if the child wanted to terminate when the parent does, can lead to rogue background processes running longer than expected
```C
pid_t pid = fork();

if (pid < 0) {
    perror("fork");
    exit(1);
} 
else if (pid == 0) {
    // Child process
    execlp("genie", "genie", "clean the house", NULL);
    
    // This part only runs if execlp FAILS
    perror("execlp");
    _exit(1); // Use _exit in child to avoid flushing parent buffers
} 

// Parent process
online_shopping();

// Wait for the child so it doesn't become a zombie
int status;
waitpid(pid, &status, 0);
```

# File APIs
- important to remember C standard library **IO APIs**
  - "f" family
    - fopen, fclose, fread, fgetc, fscanf, fprintf, etc
    - uses FILE* abstraction to represent a file x
      - user-space buffering, line-ending translation, formatted I/O, and other additional people 
- UNIX has lower-level APIs to do file handling
  - is directly mapped to **system calls**
    - open, close, read, etc.
  - use file descriptors, which are just **integers**
  - would deal with bytes only 

## Low Level APIs
- open()
  - establishes connection between file and file descriptor
    - int open(const char *path, int oflag, ...)
      - path = string representing the path to the file to open/create
      - oflag = integer that specifices access mode and other options
      - mode = required *only* when O_CREAT is used
        - uses octal number to set the file permissions for owner, group, and others 
- read() and write()
  - moves raw bytes between file descriptor and memory buffer
    - ssize_t read(int fd, void *buf, sinze_t byte) AND ssize_t write(int fc, const void *buf, size_t nbyte)
      - fd = file descriptor returned by a previous open() call
      - buf = pointer to the memory area where data is stored for the read() function or where data is being pulled for the write() function
      - nbyte = number of bytes being read or write 
- lseek()
  - used to change the current position/offset within the file where the *next* read or write operation will occur
    - off_t lseek(int fd, off_t offset, int whence)
      - fd = file descriptor for open file
      - offset = value representing the number of bytes to move the pointer
      - whence = how the offset should be interpreted
        - relative to start of file, current position, or end of file 
### Standard File Descriptors 
- has three file descriptors 
  - 0 = stdin (standard input)
  - 1 = stdout (standard output)
  - 2 = stderr (standard error)

### Flags in Open
- must include **one** of the following
  - O_RDONLY = read only
  - O_WRONLY = write only 
  - O_RDWR = read and write 
- or-ed (|) with many *optional flags* 
  - O_TRUNC = truncate a file if opening a file for write
  - O_CREAT = creates a file *if it doesnt exist*
```C
// remember open() returns -1 on error
fd1 = open("a.txt", O_RDONLY); // open for read
fd1 = open("a.txt", O_RDWR); // open for read and write
fd1 = open("a.txt", O_RDWR | O_TRUNC); // read, write, and truncates the file 
```
### Create a File With open()
```C
// a mode must be provided if O_CREAT or O_TMPFILE is set
int open(const char *path, int oflag, int mode);
```
- mode: specify permissions whe a new, or temporary, file is created
```C
open("b.txt", O_WRONLY|O_TRUNC|O_CREAT, 0600); // example of creating file with open()
// opens b.txt for write, and if it exists, clear the contents 
// if the file doesn't exist, it creates one and sets the permission so the only the owner can read and write the file 
```

# File Descriptor vs. Stream
```C
#include <stdio.h>
int fileno(FILE *stream); // returns a file descriptor for a stream
```
- either 0, 1, or 2
## File Descriptors After *forc* and *exec*
- opened files are **not affected** by the upgrade operation
```C
pid_t pid = fork();
asssert(pid >= 0);
if(pid == 0) {
  // child process can access FDs 0 (input), 1 (output), and 2 (error)
  // if execl() is successfil, gcc can access FDs 0, 1, and 2 
  execlp("gcc", "gcc", "a.c", NULL);
  // if control hets here, execlp() has FAILED
  // need to terminate the child process
}
return 1;
```

# Live Coding Examples
```C
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
  pid_t pid;
  int status;
  pid = fork();
  if (pid == -1) {
    printf("can't fork, error occurred\n");
    exit(EXIT_FAILURE);
  } else if (pid == 0) {
    printf("child process, pid = %u\n", getpid());

    char *argv_list[] = {"./test", NULL};
    /* char *argv_list[] = {"ls","-ls", NULL}; */

    execv(*argv_list, argv_list);
    /* execvp(*argv_list, argv_list); */
    exit(-1);
  } else {
    printf("parent process, pid = %u\n", getpid());
    waitpid(pid, &status, 0);

		printf("status = %d\n",status);
    printf("WIFEXITED = %d, WEXITSTATUS = %d\n", WIFEXITED(status),
           WEXITSTATUS(status));
  }
  return 0;
}
```
- describes the lifecycle of the child processes in programs 
  - forking
    - parent calls fork() 
      - if pid == 0, it identifies the child process
  - execv call
    - child prepares an arg array argv_list can calls *execv(arg_list, argv_list)*
      - first parameter is the path to the executable
      - second parameter is the array itself, and needs to be **NULL terminated**
    - if execv suceeds, it doesn't return anything because child address space is replaced with the new prorgam 
  - error handling
    - exit(-1) after execv runs only when the upgrade **fails**
  - parent role
    - parent calls *waitpid(pid, &status, 0)* to wait for the child process is finished 
      - prevents the child from being a zombie and allows parent to collect its exit status 

```C
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
int main() {
  // A null terminated array of character
  // pointers
  char *args[] = {"./exec", NULL};
  execvp(args[0], args);

  /*All statements are ignored after execvp() call as this whole
  process(execvp_demo.c) is replaced by another process (exec.c)
  */
  printf("Ending-----\n");

  return 0;
}
```
- shows how an executable takes over a process
  - process upgrade
    - program doesn't fork, but calls execvp() *directly* in the main process
  - execvp vs execv
    - execv needs the *full path*, but execvp can searc the directories in the PATH **environment variable** to find the file 
  - code replacement
    - all statements after a successful execvp call will be **ignored** since that code is no longer in the process memory