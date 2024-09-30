#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int main()
{
    int pid;
    pid = fork();
    if( pid < 0){
        perror("Fork error!");
        exit(1);
    }
    //padre e figlio eseguono lo stesso codice seguente
    printf("Process PID = %d - from fork got: %d \n", getpid(), pid);
    return 0;
}