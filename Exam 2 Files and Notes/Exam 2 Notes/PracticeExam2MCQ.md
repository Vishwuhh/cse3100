```C
int a=0;
pid_t pid = fork();

if (pid == 0)
    a++;
a--;
if(pid) wait(NULL);
printf("a=%d\n", a);
```
- **ANSWER: TRUE**
  - the (pid == 0) is the child process of the program
  - it skips the wait() line b/c pid is zero and outputs 0
    - does a++ (1) *and* a-- (0)
  - the parent would do the decrement only, but would wait for the child to finish
    - outputs a = -1
- **THINGS TO REMEMEBR*
  - memory isolation, so child chares what the parent has, but both affects it *independently*

```C
int count = 0;
if(fork())          // parent enters this branch
{
    printf("%p\n", &count);
    wait(NULL);
}
else                // child enters this branch
{
    printf("%p\n", &count);
    count++;
}
printf("%d\n", count);
```
- **ANSWER: A**
  - fork() creats a copy of the virtual address space
    - child and parent shares the *same* address for &count
  - if(fork()) is for the parent, so would print the original count (0) *after* the child
  - child would increment count, and then print (1)
- **THINGS TO REMEMBER**
  - fork() copys virtual address, so they are the same between children and parents, but **physical address** is different
  - wait() guarantees the child process finishes and dies first

```C
int main() {
    char *args[]={"ls",NULL};
    execv(args[0],args);
    printf("This is bad!\n");
}
```
- **ANSWER: TRUE**
  - execv() takes an exact path instead of searching path like execvp/execlp
    - this would fail the line, and since it fails, it would return something
- **THINGS TO REMEMBER**
  - execv requires *exact* path
  - execvp/execlp *searches* $PATH
    - if it was vp instead of v, it would have not returned anything (succeeded)

```C
int main() {
    printf("first line\n");
    int fd = open("dup2-output.txt", O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
    dup2(fd, 1);      // redirect stdout (fd 1) to the file
    close(fd);
    printf("second line.\n");
}
```
- **THE ANSWER: TRUE**
  - printf("first line /n") runs before redirect
    - it goes to stdout instead of the text, and then is shown on screen 
  - printf("second line.\n) is done after the redirect
    - the dup2(fd, 1) changes where file descriptor 1 is pointed (STDOUT to the .txt file)
      - prints there instead of on screen
- **THINGS TO REMEMBER**
  - dup2(new fd, old fd/file descriptor) redirects descriptor to point to where the fd points to
    - any new actions would go to that file instead of the original one
  - *TIMING IS IMPORTANT*

```C
int main() {
    int pd[2];
    assert(pipe(pd) != -1);
    pid_t pid = fork();

    if(pid == 0) {
    char *msg = "Hello World!\n";
    close(pd[0]);
    for(int i = 0; i < strlen(msg); i++)
        write(pd[1], &msg[i], sizeof(char));
    close(pd[1]);
    }

    close(pd[1]);
    char c;

    do {
        read(pd[0], &c, sizeof(char));
        printf("%c", c);
    } while(c!='\n');

    close(pd[0]);
    waitpid(pid, NULL, 0);
}
```
- **THE ANSWER: TRUE**
  - child writes printf("Hello World!\n") byte-by-byte into the pipe
    - closes the write end right after
  - do-while loop reads and prints each character in the pipe until it hits the '/n' chatacter
    - closes the read end after
    - waits for the child to finish its proccess
- **THINGS TO REMEMBER**
  - pipe hygiene: close ends once used
  - do-while loop reads at least once and only exits when it hits the new line
  - the parent *must* close its cope of the write end, or read would never see the EOF signal 

```C
int main() {
  #define MAX 1000000
  // ...pipe + fork...
  if(pid == 0){
      for(int i = 0; i < MAX; i++)
          write(pd[1], &i, sizeof(i));  // writes 4MB total
      close(pd[1]);
      return 0;
  }
  close(pd[1]);
  waitpid(pid, NULL, 0);  // ← waits BEFORE reading!
  int count = 0, k;
  for(int i = 0; i < MAX; i++)
      if(read(pd[0], &k, sizeof(int))) count++;
}
```
- **ANSWER: FALSE**
  - for loop runs until i = MAX (1 mil), so sizr allocated is 4,000,00 bits (4MB)
    - max that can be writed is 64KB, and then the *buffer becomes full*
  - when buffer is full, next write() call blocks until someone can read from the pipe
    - the parent is sutck in waitpid() until child is done writing, but the *child cannot write*
  - neither can make progress, so the *program hangs*
- **THINGS TO REMEMBER**
  - write() has a max buffer space of 64KB
  - always rrad from the pipe before or concurrently with waiting for the writer
    - moving waitpid() to after the reading loop would've fixed the program 

```C
main() {
  #define MAX 1000000
  int pd[2];
  assert(pipe(pd) != -1);
  close(pd[1]);         // ← write end closed BEFORE fork!
  pid_t pid = fork();
  if(pid == 0){
      for(int i = 0; i < MAX; i++)
          write(pd[1], &i, sizeof(i));  // writing to closed fd!
      close(pd[1]);
      return 0;
  }
  int count = 0, k;
  for(int i = 0; i < MAX; i++)
      if(read(pd[0], &k, sizeof(int))) count++;
}
```
- **ANSWER: FALSE**
  - the output is zero instead of 1,000,000
  - close(pd[1]) happens *before* the fork, so when it happens, pd[1] is unavailable
    - child inherits the closed pd[1], so all write() would fail since no data enters the pipe
    - parent would also have no open write end, so would immediately return zero since it hit the end of file 
- **THINGS TO REMEMBER**
  - file descriptors are inherited when fork() is called
    - closing a pipe end before fork() causes the child to have the closed pipe end as well
  - read() would return 0 in EOF, which is a *falsy*, meaning it is *not a successful read*

```C
int main() {
  if(pid == 0) {
      int i = 5;
      write(pd[0], &i, sizeof(i));  // writing to READ end!
      close(pd[0]);
      return 0;
  }
  close(pd[1]);
  int k = 0;
  int n = read(pd[0], &k, sizeof(int));
  printf("n = %d\n", n);
}
```
- **THE ANSWER: FALSE**
  - output is n = 0 instead of 4
      - child class write(pd[0], &i, sizeof(i)), but pd[0] is the *read end* of the pipe
      - leads to an error and returning -1
  - parent closes pd[1], which is the write end expecting that the child had written something in the pipe
    - read() would return 0 (EOF signal) because the write() end didnt write anuthing
  - printf("n = %d/n", 0) would print n = 9
    - n would be 54only if the write() had worked
-  **THINGS TO REMEMBER**
   - pd[2] initialized the pipe
     - pd[0] is the read end, pd[1] is the write end
   - writing to a read-only pipe *silently fails*, and when all write ends are closed, read() returns 0
  
```C
  int main() {
  if(pid == 0){
    close(pd[0]);
    char *msg = "Hello World!\nToday is a great day!\n";
    write(pd[1], msg, strlen(msg));
    close(pd[1]);
    return 0;
  }
  close(pd[1]);
  dup2(pd[0], 0);     // redirect stdin to pipe read end
  close(pd[0]);
  char s[MAX];
  while(fgets(s, sizeof(s), stdin)) printf("%s", s);
```
- **THE ANSWER: TRUE**
  - child writes the the two line message after closing the read() section of the pipe
    - closes the write ennd of the pipe as well
  - parent then closes the write end of the pipe, and uses dup2() to redirect STDIN (0) to the read end of the pupe
    - closes the read end after
    - allows fgets() to read from the pipe *transparently*
    - 1st call: reads "hello World!/n"
    - 2nd call: reads "Today is a great day!/n"
    - 2rd call: write end is closed, so returns NULL and loop exits
  - printf("%s", s) prints each line as it is since /n already exists
- **THINGS TO REMEMBER**
  - dup2() to STDIN allows fgets() or scanf() to read from the pipe *transparently*
    - dup2(pd[0], 0) needs close(pd[0]) afterwards to avoid any leaks
    - fgets() returns NULL in EOF, and is the correct loop termination condition
  
```C
int main() {
  if(pid == 0){
    close(pd[1]);
    dup2(pd[0], 0);   // child redirects its stdin to pipe
    close(pd[0]);
    char s[MAX];
    while(fgets(s, sizeof(s), stdin)) printf("%s", s);
    return 0;
  }
  close(pd[0]);
  char *s = "Hello world!\nToday is a great day!\n";
  write(pd[1], s, strlen(s));
  close(pd[1]);
  waitpid(pid, NULL, 0);
}
```
- **THE ANSWER: TRUE**
  - reverse of Q9; the child reads from the pipe and the parent writes to it
    - parent writes both lines to the pipe and closes write end
      - closes the read end before this since it doesn't need to read anything
    - child redirects stdin to the pipe itself, and then closes it
      - closes write end before since it doesn't need to write anything
  - parent closing pd[1] causes the child's fgets() to hit EOF and return NULL
    - loop exits, and the child returns
  - waitpid() in parent makes sure child process is done 