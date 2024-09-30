#include<signal.h>
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>


int stato =1, i=0;

void handler(int signo){
	stato =!stato;}

int main()
{
	printf("PId %d\n" , getpid());
	struct sigaction action, old_action;
	//Azzeriamo tutti i flag della maschera sa_mask nella struttura action
	sigemptyset(&action.sa_mask);
	action.sa_handler = handler;
	action.sa_flags =0;

	//Assegniamo action per la gestione di SIGUSR1

	if(sigaction(SIGUSR1,&action,&old_action)<0){
		perror("Sigaction error");
	}
	
	for(;;){
		if(stato){
			printf("Positivo %d\n" , i++);
		}
		else{
			printf("Negativo %d\n", i--);
		}
		sleep(1);
	}
	exit(0);
}
