#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>




// Gestore dei segnali
void handler()
{
  printf("Processo figlio %d : ho ricevuto un SIGUSR1 e termino\n",getpid());

  exit(3);
}




int main(int argc, char *argv[])
{
  sigset_t zeromask;
  struct sigaction sig;
  int exit_pid,tabpid[2],status,numeroricevuto,mionumero,n,i;
  int pp[2][2];  // Un vettore di vettori (matrice) di file descriptor


  if(argc<2 || atoi(argv[1])<=0 || atoi(argv[1])>=RAND_MAX)
    {
      printf("ERRORE negli argomenti\n");
      printf("Uso: programma <N>\n");
      printf("N deve essere maggiore di 0 e minore di RAND_MAX\n");
      exit(-1);
    }

  //Assegno n
  n=atoi(argv[1]);

  sigemptyset(&zeromask);


  //installo il gestore del segnale SIGUSR1 in modo che i figli lo ereditino
  //(in questo esercizio potrebbe essere tranquillamente fatto nel codice dei figli dopo la fork:
  // prima di inviare un segnale infatti i processi figli devono scambiarsi i messaggi sulle pipe
  // e non c'e' quindi il rischio di corse critiche
  sig.sa_handler = handler;
  sigemptyset(&sig.sa_mask);
  sig.sa_flags=0;        //non ho bisogno di flag particolari
  sigaction(SIGUSR1,&sig,NULL);


  //Creo le due pipe che i figli useranno per scambiarsi i pid e il numero casuale:
  //la pipe pp[0] verra usata in scrittura dal primo figlio, pp[1] dal secondo
  //
  //figlio1 pp[0][1] -> pp[0][0] figlio2
  //figlio2 pp[1][1] -> pp[1][0] figlio1

  if(pipe(pp[0])<0) 
    {
      perror("creazione pipe p12:");
      exit(-1);
    }

  if(pipe(pp[1])<0) 
    {
      perror("creazione pipe p21:");
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
	  // Codice eseguito dai figli
	  printf("Processo figlio %d: in esecuzione\n",getpid());

	  //metto nel *mio* vettore il mio pid
	  tabpid[i]=getpid();


	  //chiudo il lato di lettura per la pipe dove scrivo
	  close(pp[i][0]);

	  //chiudo il lato di scrittura per la pipe dove leggo
	  close(pp[(i+1)%2][1]);

	  //inizializzo il generatore di numero casuali e ne genero uno compreso tra 0 e N;
	  srand(getpid());
	  mionumero=rand() % n;
	  printf("Processo figlio %d : ho generato il numero %d\n",getpid(),mionumero);


	  //scrivo il mio pid e il numero casuale
	  if(write(pp[i][1],&tabpid[i],sizeof(int))<0)
	    {
	      perror("write pid:");
	      exit(-1);
	    }
	  if(write(pp[i][1],&mionumero,sizeof(int))<0)
	    {
	      perror("write numero:");
	      exit(-1);
	    }
	  //leggo pid e numero casuale dall'altro figlio
	  if(read(pp[(i+1)%2][0],&tabpid[(i+1)%2],sizeof(int))<0)
	    {
	      perror("read pid:");
	      exit(-1);
	    }
	  if(read(pp[(i+1)%2][0],&numeroricevuto,sizeof(int))<0)
	    {
	      perror("read numeroricevuto:");
	      exit(-1);
	    }
	  //notare come la pipe sia il punto di sincronizzazionbe tra i due figli: per fare una
	  //lettura/scrittura entrambi devono essere arrivati a questo punto

	  //se il mio numero e' minore di quello che ho ricevuto, o se sono
	  //uguali ma il mio pid e' minore, invio un SIGUSR1 all'altro figlio
	  if(mionumero<numeroricevuto || (mionumero==numeroricevuto && getpid()<tabpid[(i+1)%2]))
	    {
	      printf("Processo figlio %d : invio un SIGUSR1 al processo %d\n",getpid(),tabpid[(i+1)%2]); 
	      kill(tabpid[(i+1)%2],SIGUSR1);
	      
	    }

	  //mi metto in attesa di un segnale qualunque (lo devono fare entrambi i processi figli)

	  printf("Processo figlio %d : mi sospendo in attesa di un segnale...\n",getpid());
	  sigsuspend(&(zeromask));
	}
    }

  //Codice padre

  printf("Processo padre %d : i figli hanno PID %d e %d\n",getpid(),tabpid[0],tabpid[1]);

  //chiudo entrambe le pipe, non mi servono
  close(pp[0][0]);
  close(pp[0][1]);
  close(pp[1][0]);
  close(pp[1][1]);

  //attendo che il primo figlio termini (quello che ha ricevuto SIGUSR1 dall'altro)
  exit_pid=wait(&status);
  if(exit_pid<0)
    {
      perror("wait:");
      exit(-1);
    }
  printf("Processo padre %d: il figlio %d e' terminato\n",getpid(),exit_pid);

  //invio un SIGUSR1 al processo che e' ancora vivo
  kill( ((tabpid[0]==exit_pid) ? tabpid[1] : tabpid[0]) , SIGUSR1);
  printf("Processo padre %d: ho inviato un segnale SIGUSR1 al processo figlio %d e termino\n",getpid(),((tabpid[0]==exit_pid) ? tabpid[1] : tabpid[0]));

  exit(0);

}
 

/*
---IF aritmentico---

variabile = (condizione) ? valore1 : valore2; 

Questo e' equivalente a:

if(condizione)
  variabile=valore1:
else
  variabile=valore2;



---Resto---
L'operatore % da il resto di una divisione intera:

resto = dividendo % divisore;

ES.
0%2 = 0
1%2 = 1
2%2 = 0
3%2 = 1
4%2 = 0
5%2 = 1
ecc.

*/


