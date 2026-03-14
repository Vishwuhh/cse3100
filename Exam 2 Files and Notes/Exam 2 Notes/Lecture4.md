# Inter-Proccess Communication with Pipes

## Pipeline in Shell
- shell can support pipelines --> things that connect cmds togeter
  ```C
  cmd1 | cmd2(arg21 arg22) | cmd3(arg31)...
  ```
- stdout of the first cmd is connected to stdin of the second comand 
## Inter-process Communication (IPC)
- files, pipes, named pipes, sockets, message queues, etc. are all inter-process communication methods
  - pipes is the most **primitive** and **simple** way to do so
### *pipe()*
```C
#include <unistd.h>
int pipe(int pipefd[2]);
```
- creates a one-way pipe
  - two FDs in pipefd[]
    - pipefd[1] = write end and pipefd[0] = read end
- pipe() returns 0 if successful
### Child Process to Parent Process
- GOAL: *make child process send integer to parent process*
  - child process will write to the pipe, and parent process will read from the pipe 
  - communication is **uni-directional**
- STEPS
  - parent proccess creates the pipe
  ```C
  int p[2];
  int rv = pipe(p);
  if(rv == -1) {
    perror("pipe failed");
  }
  ```
    - p[1] is the write side, and p[0] is the read side
  - create the child process
  ```C
  int pid = fork()

  if(pid == 0) { // checks if the pid is a child
    // the child process
    // will only write into the pipe, will **not** read

    exit(0);
  }
  ```
  - remove unused FDs from the parent
    - the parent only **reads** from the pipe, it doesn't **write** anything
    - close p[1]
  ```C
  int pid = fork()

  if(pid == 0) { // checks if the pid is a child
    // the child process
    // will only write into the pipe, will **not** read

    exit(0);
  }
  // parent process
  // will only read, it will not **write**
  close(p[1]); // removes the ability to write
  ```
  - remove unused FDs from the child
    - the child only **writes** to the pipe, it doesn't **read** anything
    - close p[0] for the child 
  ```C
  int pid = fork()

  if(pid == 0) { // checks if the pid is a child
    // the child process
    // will only write into the pipe, will **not** read
    close(p[0]); // closes the read end of the pipe for the child
    exit(0);
  }
  // parent process
  // will only read, it will not **write**
  close(p[1]); // removes the ability to write
  ```
### Child Process to Child Process
- GOAL: *create two child processes, and have child 1 send a string to child 2
  - still need one pipe, and communication is uni-directional
  - child 1 will write to pipe, and child 2 will read from pipe
  - **parent doesn't do anything**
#### Connecting Two Processes
- parent create a pipe and gets two FDs (3 and 4)
  - forking leads to the child having the **same** FDs
- close the unused FDs!
#### Steps
- parent process makes the pipe
  ```C
  int p[2];
  int rv = pipe(p);
  if(rv == -1) {
    perror("pipe failed");
  }
  ```
- create first child process
  ```C
  int pid1 = fork();
  if(pid1 == 0) {
    // child process 1
    // will only write, not read

    exit(0);
  }
  ```
- create second child process
  ```C
  int pid2 = fork();
  if(pid2 == 0) {
    // child process 2
    // will only read, not write

    exit(0);
  }
  ```
- remove unused FDs in Child 1
  ```C
  int pid1 = fork();
  if(pid1 == 0) {
    // child process 1
    // will only write, not read
    close(p[0]); // removes the ability for the child to read from the pipe
    exit(0);
  ```
- remove unused FDs in Child 2
  ```C
  int pid2 = fork();
  if(pid2 == 0) {
    // child process 2
    // will only read, not write
    close(p[1]); // removes the ability for the child to write to the pipe 
    exit(0);
  ```
- remove unused FDs in Parent
  - the parent doesn't **read or write**
  ```C
  // patent process
  close(p[0]);
  close(p[1]);
  return 0;
  ```
### Note About Closing p[1] in Child 2
- no other process needs p[1] *after* child 1 since child 2 and parent doesn't write anything
  - can close p[1] right after child 1, and child 2 would never inherit

## Connecting Two Programs With a Pipe 
- start a pipleine in program S (the shell)
- high-level strategy (just misses clean up)
  - create a pipe
  - fork #1
    - in the child process
      - redirect stdout to the write end of the pipe 
      - start A by calling exec
  - fork 2
    - in the child process
      - redirect stdin to the read end of the pipe 
      - start B by calling exec
## FDs of a Dying Process
- all FDs are automatically closed when a process ends 
- *what happens to the processes on the other end of the pipe?*
  - assume S does not read or write, but have the FDs of the pipe
    - if both processes A (writes) and B (reads) die, B gets EOF when the buffered data is consumed 
    - if A dies, B will wait for more data (if S can write into it)
    - if both B and S die, A gets a SIGPIPE error when writing
    - If B dies, A will wait if the pipe is full (assuming S can read the contents in it)
## Going Further
- can repeat process to make a long pipeline
- use pictures to visualize the way pipes are used
- processes are running in parallel once created 
- all rpocesses in the pipeline run concurrently on Linux
  - when the data is sent in the pipe, the next process can pick it up and start to work 
## Atomicity of *read()* and *write()*
```C
nr = read(fd, buf, N);
nw = write(fd, buf, N);
```
- write() and read() returns the num of bytes read/written
  - returned values may be *less* than the requested
- atomicity of write() is guaranted if the number of bytes is *less* than PIPE_BUF
  - bytes would be consecutive, and the default value of PIPE_BUF is 4096 in Linux
- for read(), it is fine if all writes and reads are of the same side
  - if not, needs special handling
## Child and Parent Process Communication
- GOAL: *make parent process send integer n and child process would return sum of n natural numbers*
- in child process
  - read the value n from pipe
  - compute sm
  - write the result into pipe
- in parent process
  - write the n value from pipe
  - read the result from the pipe
## Why Can't We Just Have One Pipe for Two-Way Comms?
- we can't figure out which process would the the first one to read from a pipe if they do it at the **same time**
  - two processes race for data, but we can prevent this using things like wait() and waitpid()
- the simpler way to do this is through two pipes
  - one for sending information each direction
    - has its dangers too, needs to make sure both processes are not trying to read from empty pipes at the same time 
## Starting a 2-Stage Pipeline
```C
// A | B (process A is piping to process B)

pipe(pipefd); // pipefd is an array of 2 ints
pid_a = fork();  // child process for A
if(pid_a == 0) {
  dup2(); // sets up stdout for A
  // close both FDs in pipefd exec to start A
}
close(pipefd[WR_END]); // doesn't need to keep it open in parent 

pid_b = fork()
if(pid_b == 0) {
  dup2();
  close(pipefd[RD_END]);
  // exec to start B
}
close(pipefd[RD_END]);
```

# Lecture Dump
- $ valgrind --track-fds=yes ./p
  - shows the open file descriptors at the end of the program 
- cat doc.txt | tr a-z A-Z
  - how many processes? two
    - one process for cat and one process for tr
  - takes STDOUT from (cat doc.txt) and uses that as STDIN for tr
- shell supports pipleine, which chains commands together 
- interprocess communication can be done in many ways
  - files, pipes, named pipes, sockets, message queues, shared memory, etc. 
    - pipes are the most primitive way to do so
  - sychonization primitives 
- pipe()
  - pipefd[0] is the READ end, pipefd[1] is the WRITE end
  - returns 0 if successful
  - going from parent -> child
    - parent writes in pipefd[1] and child reads from pipefd[0]
  - communication is unidirectional
```C
int pipe(int pipefd[2]); // creates one-way pipe
```
- steps for child -> parent 
  - parent process first creates a pipe
    - p[1] for write, p[0] for read
  - create a child process for fork
    - int pid = fork() and if(pid == 0) to identify child 
    - child should only write, not read 
    - p[1] is write and p[0] is read too
  - remove unsused file descirptors in parent
    - parent is only reading, so close the read section p[1]
      - close(p[1])
  - remove unused filed descriptors in child
    - child is only writing, so close the wrtie section p[0]
      - close(p[0])
  - NOTE: close after writing/reading for the respective processes too
- steps for communicating between two **children** child1 -> child2
  - we have a parent process, but we aren't doing anything with it --> doesn't read or write
  - parent process creates pipe
    - p[1] for write, p[0] for read
  - create child process 1 with fork
    - p[1] for write, p[0] for read
  - create child process 2 with fork
    - p[1] for write, p[0] for read
  - remove file descriptros in child1
    - remove p[0] since it is supposed to only write
  - remove file descriptors in child2
    - remove p[1] since it is supposed to only read
  - remove file descriptors in parent
    - not supposed to do anything, so remove p[1] and p[0]
- can close p[1] earlier than p[0] because child1 is the only one that needs it