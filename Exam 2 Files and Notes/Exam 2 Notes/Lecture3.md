# Redirection
-  function open() reutrns a non-negtive int which acts as a descriptor
   -  used in read(), close(), etc.
   -  0 = stdin, 1 = stdout, 2 = stderr
-  files opened in proccess *remain* open after fork() and err

## Shell Redirections
- can use when executing commands in shell (ex. bash)
  - **implemented with close/open/dup technique**
```C
$ command < infile > outfile 
// < infile: takes input from file infile
// > outfile: sends output to file outfile
```
- additional variants
  - '>>' outfile = appends output to file outfile
    - quotes added for markdown
  - 2> outfile = sends errors to outfile
  - &> outfile = sends output and errors to outfile 
  - look into /dev/null
```C
$ sort < file.txt > sorted.txt
// sort reads from file.txt instead of terminal
// output of sort is saved in sorted.txt, so cannot be seen on screen
```
- statements in sort arent changed
  - **read from stdin, prints to stdout**

## File Descriptor Table
- all proccess have **file descriptor tables**, and holds indices into the Open File Table for the OS
  -  OFT hilds mode of all opened files, and holds index of the Inode Table with the name and location on disk

## Duplicating File Descriptors
- doesn't change the descriptor table directly, but uses function calls
- dup() and dup2()
  - dup() copies oldfd into the first available entry in the table
  - dup2() copies oldfd into a specified new fd
    - closes newfd if it is in use
```C
#include <unistd.h>

int dup(int oldfd);
int dup2(int oldfd, int newfd);
```

## Redirecting stdout
- open() a file and save the file descriptor in fd
- dup2()
  - copy fd to 1 so the file descriptor 1 points to the just opened file
- close(fd)

## Implementing Redirections 
- how does bash do redirection for other processes?
  - bash starts a new process, and does fork() and exec
    - the file descriptor table is **preserved through these**
- idea
  - in child processes, set up the file descriptors *before* ipgrading
    - change the files corresponding to stdin, stdout, or stderr

# Lecture Dump Notes
- how does ls -l works in the backend
  - shell creates a child process in Linux
    - leads execvp("ls", "l", ...);
    - prints the list of file into stdout 
- what happens when we do ls -l > lsout.txt
  - when doing cat lsout.txt prints the contents of lsout
  - puts the things that would be in stdout *into* lsout with the greater sign
  - how is ls aware of the file?
    - ls writes to file descriptor 1, and that usually points to stdout
- shell redirections
  - we are trying to change *what* the value 1 points to, even if the program isn't aware of the file being pointed to 

- file descriptor table
  - holds indices of entries into the **open file table** managed in OS
    - OPT holds the mode of the opened file (read, write, etc)
  - can point to successibe tables
- child processes inhereits the parents open files --> would hold the *same* file descriptors as well
- duplicating file descriptors
  - fd = file descriptor
  - dup() copies oldfd to the **first available entry** in the FD table
  - dup2() copies oldfd to newfd, and closes newfd first if it is in use
  - the two dile descriptors are different, but since they are associated to the same file, they both affect it
- using dup and dup2
  - can make sure that outputs could be pointed to a .txt or any other file **instead** of terminal if needed
  - can use it wwith open() and close() too
    - dup2() combines dup and close into one line 
    - dup2(fd, 1) = closes 1, and duplicates fd to 1 and has it point to where the original output
    - close the duplicated fd after dup-ing to avoid vulnerabilities 
- redirecting stderr
  - how to clode FD 3 in child?
    - close(3) if a == 0
- if a proccess wants to start a new program and wants to redirect stdout to a new file, do it **after fork() but before stdout**
- NOTE: shift operator converts **characters to integers**
- wc() shows number of lines, words, and bytes taken up in memory