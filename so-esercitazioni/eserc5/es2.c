#include <signal.h>
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
    {
      /* il figlio dorme fino a quando riceve il segnale SIGKILL, poi muore*/
      for (;;)
       printf("sono il figlio e sto ciclando all'infinito!\n");
      printf("questo messaggio non dovrebbe mai essere visualizzato!\n");
      exit(0);
    }
  else
    {
      /* il padre invia un segnale SIGKILL al figlio */
      sleep(3);
      kill(pid, SIGKILL);
      printf("\nsono il padre e ho ucciso il figlio!!\n");
      exit(0);
    }
}
