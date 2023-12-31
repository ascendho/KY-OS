// Compile: gcc -o fork2 fork2.c
// Run:     ./fork2

#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>

int main() {
    pid_t pid;

    /* fork another process */
    pid = fork();
    if (pid < 0) {                        /* error occurred */
        fprintf(stderr, "Fork Failed");
        return 1;
    } else if (pid == 0) {                /* child process */
        execlp("/bin/ls", "ls", NULL);
    } else {                              /* parent process */
        wait(NULL);                       /* parent will wait for the child to complete */
        printf("Child Complete\n");
    }

    return 0;
}