#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>


int main()
{
  pid_t pid;

// --- replicare
  if ((pid=fork()) < 0)
    {
      perror("fork error");
      exit(1);
    }  
  printf("hello from %d!\n", pid);
// --- questo blocco  
  
  exit(0);
}
