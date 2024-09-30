#include <stdio.h>    //printf
#include <stdlib.h>   //exit
#include <signal.h>   //signal
#include <unistd.h>   //sleep

void catchint(int signo)
{
  printf("catchint: signo = %d\n", signo);
  /* non si prevedono azioni di terminazione: ritorno al segnalato */ 
}


int main()
{
  signal(SIGINT, catchint);
  for (;;) 
    {
      printf("In attesa del segnale SIGINT (premere ctrl^c)\n");
      sleep(1);
    }

  exit(0);
}
