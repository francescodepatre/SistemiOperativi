#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

pid_t pidpadre;
int tabpid[2];


sigset_t zeromask;

// Gestore dei segnali
void handler()
{
  // Lo stesso gestore per il padre e i due figli
  
  printf("Figlio (PID=%d) : ho ricevuto un SIGUSR1 e termino tra 3 secondi\n",getpid());
  
  sleep(3);
  exit(0);
      
}


figlio(int i, int pfd)
{
  
  read(pfd, tabpid, sizeof(tabpid));
       
  kill(tabpid[1-i],SIGUSR1);

  system("ps");
  
  sleep(2);
  printf("Figlio (PID=%d) termina\n", getpid());

}



int main(int argc, char *argv[])
{

  struct sigaction sig;
  int exit_pid,status,id,i;
  int pp[2][2];  // Un vettore di vettori (matrice) di file descriptor

  pidpadre= getpid();

  //installo il gestore del segnale SIGUSR1 in modo che i figli lo ereditino
  //(in questo esercizio potrebbe essere tranquillamente fatto nel codice dei figli dopo la fork:
  // prima di inviare un segnale infatti i processi figli devono scambiarsi i messaggi sulle pipe
  // e non c'e' quindi il rischio di corse critiche
  sig.sa_handler = handler;
  sigemptyset(&sig.sa_mask);
  sig.sa_flags=0;        //non ho bisogno di flag particolari
  sigaction(SIGUSR1,&sig,NULL);


  //Creo le 2 pipe 

  if(pipe(pp[0])<0) 
    {
      perror("creazione pipe prima pipe:");
      exit(-1);
    }

  if(pipe(pp[1])<0) 
    {
      perror("creazione pipe seconda pipe:");
      exit(-1);
    }



  //Creo i figli
  for(i=0;i<2;i++)
    {
      if((tabpid[i]=fork())<0) 
	{
	  perror("fork:");
	  exit(-1);
	}
      if(tabpid[i]==0)
	{
	  figlio(i,pp[i][0]);
	  exit(0) ;
	}
    }

  /* Selezione del figlio a cui inviare il messaggio */
  id = getpid() % 2 ;

  write(pp[id][1], tabpid, sizeof(tabpid));
	
  sleep(10);
	
  printf("Padre (PID=%d) : termino\n",getpid());

}

