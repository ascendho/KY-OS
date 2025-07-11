#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/wait.h>
#include <stdlib.h>
int main()
{
   pid_t childpid; /* variable to store the child's pid*/
   int retval;     /* user-provided return code for child process*/
   int status;     /* child's exit status for parent process*/

   /*create new process*/
   childpid=fork();
   if(childpid>=0) /*fork succeeded*/
   {
      if (childpid==0) /*fork() return 0 to the child process*/
      {
        printf("CHILD: I am the child process! \n");
        printf("CHILD: Here's my PID: %d\n", getpid());
        printf("CHILD: My parent's PID is: % d\n", getppid());
        printf("CHILD: The value of fork return is: % d\n", childpid);
        printf("CHILD: Sleep for 1 second...\n");
        sleep(1);
        printf("CHILD: Enter an exit value (0~255): ");
        scanf("%d",&retval);
        printf("CHILD: Goodbye! \n"); 
        exit(retval); /*child exits with user-provided return code.*/
      }
      else  /*fork() returns new pid to the parent process */
      {
        printf("PARENT: I am the parent process! \n");
        printf("PARENT: Here's my PID: %d\n", getpid());
        printf("PARENT: The value of my child's PID is: %d\n", childpid);
        printf("PARENT: I will now wait for my child to exit.\n");
        wait(&status); /* wait for child to exit,and store its status*/
        printf("PARENT: Child's exit code is: %d\n", WEXITSTATUS(status));
        printf("PARENT: Goodbye! \n"); 
        exit(0); /* parent exits */ 
      }
  }
  else /* fork returns -1 on failure */
  {
     perror("fork error!"); /*display error messsage*/
     exit(0);
   }
}
       

