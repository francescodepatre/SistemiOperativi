#include <unistd.h>
#include <stdio.h>

int main()
{
    printf("Sono il processo con PID=%d e sono figlio del processo con PID=%d\n", getpid(), getppid());
}