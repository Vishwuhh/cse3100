## Monday Review
- child is ran with pid == 0, and parent would be the else case
  - OR pid > 0 
  - good for spliting tasks for parent and child 
- waitpid and wait
  - wait would wait for the first child process to exit
  - waitpid would wait for the specific child process to exit
  - both help with making sure there are no zombies/make the order of tasks
      - they can take status into account as well, but if you don't need that, you can set it to NULL
- exec
  - replace current process of memory with the thing you called, and then run sit 
  - successful exec calls **NEVER** returns anything if successful 
  - execl, execv, execlp, execvp
    - if given simple execl and simple print statement
      - the done with command never goes and does the print statement normally
      - set a prev variable a to fork(), and does a child process if statement with the execl()
          - the parent process would do the print statement normally
- /home/user/code/calc 
  - calc takes three arguments 
  - the executable to run
  - could you write an exec code to make sure that the program can run that executable no matter where in the folder it is in?
    - execl("path of executable", "give the path AGAIN", "sum", "1", "2", "NULL")
    - the restating of the path avoids the issue of ./calc since it asks the line to look inside the **CURRENT** folder and find the executable
    - can also do execv(path, *args[]) where (char *args) is the array of arguments in the executable 
  - execlp looks within the path you are on for the arguments
    - same for execvp
- int fd = open("text.txt", O_RW(insert))
  - can have different levels of access for READ, WRITE, or BOTH
- file descriptors
  - 0 = stdin, 1 = stdout, 2 = stderr, (rest is up to program)
    - int fd = open("text.txt", O_RW(insert)) would have file descriptor of three
  - fd' = dup(fd)
    - duplicates the file descriptor to the next available slot on the FDT
  - you can change where the file descriptors point to
    - close(old file descriptor), and do dup(fd)
      - since dup() goes to the **first** avaliable descriptor, it would point the new file to the old FD
      - can do close(fd) now that it had been duplicated
    - dup2(fd, target file descriptor) can take the first two steps into one
      - would still need to do close(fd) manually
  - write() can write 4096 bytes at **MOST**, and if the data is greater than that, it does it in chunks
## Free Response Practice Question
```C
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

void redirectStdin(*const char *filename) { // helps redirect standard input (STDIN) to a file 
  int fd = open(filename, O_RDONLY); // opens source file in RDONLY
  if(fd < 0) { // error for missing files
    perror("Error opening the file\n");
    exit(-1);
  }
  // dup2(oldfd, newfd) makes newfd also point in the same place as oldfd
  // STDIN_FILENO is zero, so we force the program to read from our file instead of user input
  dup2(fd, STDIN_FILENO);
  // close the original fd, since it would be a duplicate
  close(fd);
}

void redirectStdout(const char *filename) { // redirects STDIN to a file of our choice
  // O_WRONLY = write only
  // O_CREAT = create file if it doesn't exist
  // O_TRUNC = overwrites existing content
  // S_IRUSR | S_IWUSR = sets permissions for user to read/write
  int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR );
  if(fd < 0) { // handles errors where the file doesn't exist
    perror("Error opening the file\n");
    exit(-1);
  }
  // STDOUT_FILENO is one, so we force thr program to write to out file instead of terminal 
  dup2(fd, STDOUT_FILENO);
  close(fd);
}
```

```C
int main(int argc, char *argv[]) {
  // the program expects two arguments (input file and output file), so this valudates it
  if(argc != 3) {
    printf("%s filename file_result\n", argv[0]);
    return(0);
  }
  // define the command and the arguments
  // "tr" is the command, "a-z" and "A-Z" is the translation sets
  char *argv_list[] = {"tr", "a-z", "A-Z", NULL};
  // redirect STDIN to the file in the first argument
  redirectStdin(argv[1]);
  // redirect STDOUT to the file in the second argument
  redirectStdout(argv[2]);
  // execvp looks for "tr" in the system path and replaces the current process with it, passing through argv_list
  execvp(argv_list[0], argv_list);
  // execvp only returns if an error occurs, which is BAD
  perror("Something went wrong\n");
  return(-1);
}
```

```C
void run_simlation(int n, double p) { // need two pipes per child (one to send commands, one to get results)
  int pA_to_P[2], P_to_pA[2]; // player A pipes
  int pB_to_P[2], P_to_pB[2]; // player B pipes 

  pipe(pA_to_P); pipe(P_to_pA); // creating player A pipes
  pipe(pB_to_P); pipe(P_to_pB); // creating player B pipes

  // creating Player A
  if(fork() == 0) {
    srand(time(NULL)^(getpid() << 16)); // seeds random generator for the process
    int cmd, res;
    // blocks the child process until parent is done
    // parent will write to P_to_pA[1] the vale of 1
    while(read(P_to_pA[0], &cmd, sizeof(int)) > 0) { // reads from pipe A what the parent says
      res = ((double)rand() / RAND_MAX) ? 1 : 0; // short hand for an if-else statement 
      // ssize_t write(int fd, const void *buf, size_t count)
      // *buf is a pointer to the location of where the data currently is
      // size_t count is the sexact number of bytes to cound (can use sizeof() in this)
      write(P_to_pA[1], &res, sizeof(int)); // writes to pipe A
    }
    exit(0);
  }
  if(fork() == 0) {
    srand(time(NULL)^(getpid() << 16)); // seeds random generator for the process
    int cmd, res;
    // blocks the child process until parent is done
    // parent will write to P_to_pB[1] the value of 1
    while(read(P_to_pB[0], &cmd, sizeof(int)) > 0) { // reads from pipe B what the parent says
      res = ((double)rand() / RAND_MAX) ? 1 : 0; 
      write(P_to_pB[1], &res, sizeof(int)); // writes to pipe B
    }
    exit(0);
  }

  int a_wins = 0;
  for(int i - 0; i < n; i++) { // loops for n rounds
    while(1) { // loops within a round until someone gets a head
      int signal = 1;
      // parent signals to A
      write(P_to_pA[1], &signal, sizeof(int)); 
      int resA;
      read(pA_to_P[0], &resA, sizeof(int));
      if(resA == 1) { // increases the wins for A by 1, and ends the cycle
        a_wins++;
        break;
      }
      // occurs if A gets a tail, so B automatically wins
      write(P_to_pB[1], &signal, sizeof(int));
      int resB;
      read(pB_to_P[0], &resB, sizeof(int));
      if(resB == 1) {
        break; //exits the loop because round is over
      }
    } 
  }
  // closes the write end of the command pipes
  // sends end of file signal to children
  close(P_to_pA[1]);
  close(P_to_pB[1]);
  // waits for the children to see the signal, finishing their loops, and exits
  // prevents zombification
  wait(NULL);
  wait(NULL);
  // closes the read ends of the parents
  close(pA_to_P[0]);
  close(pB_to_P[0]);
}
```