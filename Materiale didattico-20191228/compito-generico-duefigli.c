#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>

#define NFIGLI 2

/******
Padre crea due figli, genera un numero casuale e se è pari il secondo figlio
manda un segnale SIGUSR1 al primo, se è dispari, viceversa.

******/

int segnale_ricevuto;

void sigusr_handler (int signo) {
    segnale_ricevuto = signo;
    printf("FIGLIO PID %d : (gestore SIGUSR1) ricevuto segnale\n",getpid());
}


main()
{
int piped[2];
int i,pid,tabpid[2], moneta;

struct sigaction act;
sigset_t sigmask, zeromask;


/* GESTIONE SEGNALI  */

sigemptyset( &zeromask);
sigemptyset( &sigmask);
sigaddset(&sigmask, SIGUSR1);

/* Blocco del segnale SIGUSR1 */
sigprocmask(SIG_BLOCK, &sigmask, NULL);

act.sa_handler= sigusr_handler; 
sigemptyset( &act.sa_mask);
act.sa_flags= 0;

sigaction(SIGUSR1, &act, NULL);

/* L'intera politica di gestione dei segnali verra' ereditata del figlio */


/* CREAZIONE PIPE  */
  if(pipe(piped)<0)
    {
      perror("Creazione pipe :");
      exit(-1);
    }

/* CREAZIONE FIGLI  */
for (i=0;i<NFIGLI;i++) {
    if((tabpid[i]=fork())<0) {
        perror("Creazione processo figlio (fork) :");
        exit(-2);
        }
    else
        if(tabpid[i]==0)   /* Processi figlio */ {
            
        // Entrambi i figli leggeranno il pid inviato dal padre
        read(piped[0],&pid,sizeof(pid_t));
        
        if (pid == getpid()) {
                // Il processo corrente e' il destinatario del segnale
                sigsuspend(&zeromask);
                printf("FIGLIO PID %d: ricevuto il segnale %s\n",getpid(),(segnale_ricevuto== SIGUSR1)? "SIGUSR1":"segnale ?\n");
        }
        else {
            // Il processo corrente e' il mittente del segnale
            kill(pid,SIGUSR1);
            printf("FIGLIO PID %d: inviato il segnale SIGUSR1\n",getpid());
        }
            
            
        printf("FIGLIO PID %d: termino\n",getpid());
        
        exit(0);
      }
    }      

// Inizializza il generatore di numeri casuali    
srand(getpid());
moneta = rand() & 1;
printf("La moneta dice %d\n",moneta);
if (moneta) {
        write(piped[1],&tabpid[1],sizeof(pid_t));
        write(piped[1],&tabpid[1],sizeof(pid_t));

}
else {
        write(piped[1],&tabpid[0],sizeof(pid_t));
        write(piped[1],&tabpid[0],sizeof(pid_t));
}
for (i=0;i<NFIGLI;i++)
        wait(NULL);
printf("PADRE PID %d: termino\n",getpid());
    
  
    
}
