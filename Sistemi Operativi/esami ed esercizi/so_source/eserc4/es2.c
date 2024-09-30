#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
  int pid;		

  if ((pid=fork()) < 0)
    {
      perror("fork error");
      exit(1);
    }
  else
   if (pid == 0)		
    {/* CODICE ESEGUITO DAL FIGLIO */
      printf("Sono il processo figlio con PID=%d\n", getpid());
    }
  else
    {/* CODICE ESEGUITO DAL PADRE */
      printf("Sono il processo padre con PID=%d e ho generato un figlio che ha PID=%d\n", getpid(),pid);
    }
}
