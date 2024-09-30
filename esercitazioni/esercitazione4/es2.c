#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
    int pid;
    pid = fork();

    if (pid < 0)
    {
        perror("Fork error");
        exit(1);
    }
    else if (pid == 0)
    {
        //codice del figlio
        printf("I'm child process with PID = %d\n", getpid());
    }
    else
    {
        //codice del padre
        printf("I'm parent process with PID = %d\n", getpid());
        printf("I generated child process with PID = %d\n", pid);
    }
    return 0;
}