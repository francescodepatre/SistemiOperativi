#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>


 
int main()
{
int pid;

if ((pid=fork()) < 0) {
    perror("Errore fork");
    exit(1);
    }
    
/* Entrambi i processi eseguono la printf */
printf("Processo PID=%d - dalla fork ho ottenuto %d\n",getpid(),pid);
}
