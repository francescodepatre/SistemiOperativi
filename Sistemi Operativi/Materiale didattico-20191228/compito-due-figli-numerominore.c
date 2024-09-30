#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>

#define NFIGLI 2

#define NDEFAULT 100

/******
Padre crea due figli, genera un numero casuale e se è pari il secondo figlio
manda un segnale SIGUSR1 al primo, se è dispari, viceversa.

******/

int segnale_ricevuto;

void sigusr_handler (int signo) {
    segnale_ricevuto = signo;
    printf("FIGLIO PID %d : (gestore SIGUSR1) ricevuto segnale\n",getpid());
}


main(int argc,char *argv[])
{
int pipedsc[2],pipedcs[2];
int i,n, num,pid,tabpid[2];
int mesg[2],mesg2[2];

struct sigaction act;
sigset_t sigmask, zeromask;

if (argc != 2)  {
    printf("Uso: %s N\n",argv[0]);
    exit(1);
}
    
n = atoi(argv[1]);
if (n <=0) 
    n = NDEFAULT;


/* GESTIONE SEGNALI  */

sigemptyset( &zeromask);
sigemptyset( &sigmask);
sigaddset(&sigmask, SIGUSR1);

/* Blocco del segnale SIGUSR1 */
sigprocmask(SIG_BLOCK, &sigmask, NULL);

act.sa_handler= sigusr_handler; /* Lo stesso gestore per entrambi i segnali
 */
sigemptyset( &act.sa_mask);
act.sa_flags= 0;

sigaction(SIGUSR1, &act, NULL);

/* L'intera politica di gestione dei segnali verra' ereditata del figlio */




/* CREAZIONE PIPE  */

// Soluzione con due pipe 

  // Pipe pipeds per comunicare dai client al server
  if(pipe(pipedcs)<0)
    {
      perror("Creazione pipe :");
      exit(-1);
    }
    
  // Pipe pipedc per comunicare dal server ai client
  
  if(pipe(pipedsc)<0)
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
            
        // Inizializza il generatore di numeri casuali    
        srand(getpid());    
            
        num = rand() % (n+1);
        pid = getpid();
        mesg[0] = pid;
        mesg[1] = num;
        
        write(pipedcs[1],mesg,sizeof(mesg));

           
        read(pipedsc[0],&pid,sizeof(pid_t));
        
        if (pid == getpid()) {
                sigsuspend(&zeromask);
                printf("FIGLIO PID %d: ricevuto il segnale %s\n",getpid(),(segnale_ricevuto== SIGUSR1)? "SIGUSR1":"segnale ?\n");
        }
        else {
            kill(pid,SIGUSR1);
            printf("FIGLIO PID %d: inviato il segnale SIGUSR1\n",getpid());
            sigsuspend(&zeromask);
            printf("FIGLIO PID %d: ricevuto il segnale %s\n",getpid(),(segnale_ricevuto== SIGUSR1)? "SIGUSR1":"segnale ?\n");
        }
            
            
        printf("FIGLIO PID %d: termino\n",getpid());
        
        exit(0);
      }
    }      

read(pipedcs[0],mesg,sizeof(mesg));
read(pipedcs[0],mesg2,sizeof(mesg2));

printf("Padre: ricevuto %d (da PID %d) e %d (da PID %d)\n",mesg[1],mesg[0],mesg2[1],mesg2[0]);


if (mesg2[1] <= mesg[1]) {
        printf("Padre: figlio PID %d ricevera' per primo il segnale\n",mesg[0]);
        write(pipedsc[1],&mesg[0],sizeof(pid_t));
        write(pipedsc[1],&mesg[0],sizeof(pid_t));
}
else {
        printf("Padre: figlio PID %d ricevera' primo segnale\n",mesg2[0]);
        
        write(pipedsc[1],&mesg2[0],sizeof(pid_t));
        write(pipedsc[1],&mesg2[0],sizeof(pid_t));
}
wait(NULL);
if (mesg2[1] <= mesg[1]) {
    printf("Padre: invio segnale SIGUSR1 a figlio PID %d\n",mesg2[0]);
    kill(mesg2[0],SIGUSR1);
}
else {
    printf("Padre: invio segnale SIGUSR1 a figlio PID %d\n",mesg[0]);
    
    kill(mesg[0],SIGUSR1);
}

wait(NULL);
printf("PADRE PID %d: termino\n",getpid());
    
  
    
}
