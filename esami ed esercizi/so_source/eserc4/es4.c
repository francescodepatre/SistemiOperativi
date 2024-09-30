#include <unistd.h>
#include <sys/types.h>

#include <stdio.h>
#include <stdlib.h>

int main()
{
  pid_t pid;

  if ((pid=fork()) < 0)
    {
      perror("fork error\n");
      exit(1);
    }  
  printf("hello from %d!\n", pid);
  exit(0);
}
