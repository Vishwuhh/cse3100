#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char ** argv)
{
    pid_t child = fork();
    int exitStatus;

    // moved since it could cause a SegFault
    if (argc < 4) { 
        fprintf(stderr, "Usage: %s cmd1 cmd1_arg cmd2 [cmd2_args ..]\n", argv[0]);
        return 1;
    }

    // at least, there should be 3 arguments
    // 2 for the first command, and the rest for the second command
    if(child == 0) { 
    execlp(argv[1], argv[1], argv[2], NULL);
    perror("excelp()");
    exit(1);
    }
    else if (child < 0) {
        perror("fork()");
        exit(1);
    }

    int status;
    waitpid(child, &status, 0); 

    printf("exited=%d exitstatus=%d\n", WIFEXITED(status), WEXITSTATUS(status));

    pid_t pid_2 = fork(); // new fork for a different operation
    if(pid_2 == 0) {
        execvp(argv[3], &argv[3]);
        perror("execvp()");
        exit(1);
    }
    else if (pid_2 < 0)
    {
        perror("fork()");
        exit(1);
    }
    int status2;
    waitpid(pid_2, &status2, 0);
    
    printf("exited=%d exitstatus=%d\n", WIFEXITED(status2), WEXITSTATUS(status2));

    return 0;
}
