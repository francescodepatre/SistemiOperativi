#include<signal.h>
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>

int ntimes =0;

void catcher(int signo){
	printf("Processo %d ricevuto %d volte\n" , getpid(), ++ntimes);
}

int main()
{
	int pid,ppid;
	sigset_t set,zeromask;
	struct sigaction action;
	
	sigemptyset(&zeromask); //prepara una maschera di segnali vuota da usare più avanti
	
	sigemptyset(&action.sa_mask);
	/* azzera tutti i flag della mschera di segnali sa_mask, presente allinterno della struttura action
	Tale maschera corrisponde all'insieme di segnali che saranno bloccati durante l'esecuzione della
	procedura di gestione del segnale a cui action verrà associata */

	action.sa_handler = catcher;

	action.sa_flags =0;

	//PROTEGGO IL PROESSO DALL'ARRIVO DI SEGNALI SIGUSR1 AL DI FUORI DEL MOMENTO IN CUI LO ATTENDO
	//IL momento in ci lo attendo corrisponde al punto in cui pongo la sigsuspend	
	sigemptyset(&set); //azzera la maschera di segnali set
	sigaddset(&set, SIGUSR1); // aggiungo SIGUSR1 alla maschera di segnali set
	sigprocmask(SIG_BLOCK, &set , NULL); //aggiunge in or all'insieme di segnali mascherati dal processo corrente quelli contenuti in set

	if(sigaction(SIGUSR1, &action, NULL)==-1){
		//assegna action per la gestione di SIGUSR1
		perror("Sigaction error");}
	/*Da questo punto in oi, alla ricezione di un segnale SIGUSR1 il processo corrente (come anche i processi figli)
	risponderà con l'invocazione della funzione catcher(). In queso momento però i sengali SIGUSR1 sono invisibili al processo grazie 
	alla precedente invocazione della sigprocmask: grazie alla sigsuspend(&zeromask) sblocco i segnali nel momento in cui sono pronto a 		riceverli. */

	if ((pid=fork())<0){
		perror("fork error");
		exit(-1);}
	
	if(pid==0){
		ppid = getppid();
		printf("Sono il figlio %d , mio padre è %d\n" ,getpid() , ppid);
		for(;;){
			sleep(1);
			kill(ppid, SIGUSR1);
			sigsuspend(&zeromask); //ci sospendiamo fino alla ricezione di un segnale qualunque
		}
	}
	
	if(pid>0){
		printf("Sono padre %d , mio figlio è %d\n", getpid(), pid);
		for(;;){
			sigsuspend(&zeromask);	
			sleep(1);
			kill(pid,SIGUSR1);
		}
	}
}





			






	
	
