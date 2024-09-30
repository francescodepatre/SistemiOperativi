#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
    int pid, status, myvar;

    myvar = 1;
    pid = fork();
    if (pid < 0)
    {
        perror("Fork error");
        exit(1);
    }
    else if (pid == 0)
    {
        printf("I'm child process %d, son of process %d\n", getpid(), getppid());
        printf("Gonna wait 2 seconds...\n");
        sleep(2);
        myvar = 2;
        printf("Son myvar = %d\n", myvar);
        exit(0);
    }
    else
    {
        wait(&status);
        WEXITSTATUS(status);
        printf("I'm parent process %d, son of process %d\n", getpid(), getppid());
        printf("Parent status = %d\n", status);
        printf("Parent myvar = %d\n", myvar);
        exit(0);
    }
    return 0;
}