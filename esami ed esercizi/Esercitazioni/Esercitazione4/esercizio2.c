#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<signal.h>


void handler(int signo){	
	printf("E' stato ricevuto un segnale = %d \n" , signo);
	exit(0);
}


int main(){
	int pid;
	
	if((pid=fork())<0){
		perror("Fork() error");
		exit(-1);
	}

	if(pid ==0){
		for(;;)
		{
			printf("Sono il processo figlio con pid %d e sto ciclando all'infinito in attesa del segnale\n" , getpid());
			signal(SIGUSR1,handler);
			sleep(1);
		}
		printf("Questo messaggio non dovrebbe essere mai visualizzato\n");	
		exit(0);
	}
	
	else {
		
		sleep(3);
		kill(pid,SIGUSR1);
		printf("\nSono il padre con pid %d e ho ucciso mio figlio con pid %d\n" , getpid() , pid);	
		exit(0);
		}
	return 0; 
}
