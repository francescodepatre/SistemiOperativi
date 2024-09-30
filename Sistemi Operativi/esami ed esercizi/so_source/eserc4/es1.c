#include <unistd.h>
#include <stdio.h>


 
int main()
{
  printf("Sono il processo %d e sono figlio di %d\n", getpid(), getppid());
}
