#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
int main()
{
    pid_t pid;
    pid = fork();
    if (pid < 0)
    {
        perror("Errore fork");
        exit(1);
    }
    printf("ciao, pid vale %d!\n", pid);
    exit(0);
}