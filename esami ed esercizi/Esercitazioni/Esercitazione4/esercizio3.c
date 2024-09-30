#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<signal.h>


int ntimes =0;

void catcher(int signo){
	printf("Processo %d ricevuto %d volte\n" , getpid(), ++ntimes);
	}

int main(){
	int pid, ppid;
	signal(SIGUSR1, catcher);
	if((pid=fork())<0){
		perror("Fork() error");
		exit(-1);
	}
	if(pid ==0){
		ppid = getppid();
		printf("Sono il figlio con pid %d e mio padre è %d\n" , getpid() , ppid);
		
		for(;;){
			sleep(1);
			kill(ppid,SIGUSR1);
			pause();
		}
	}
	else{
		printf("Sono il padre con pid %d e mio figlio è %d\n" , getpid() , pid);
		for(;;)
		{
			pause();
			sleep(1);
			for(int i=0; i<ntimes; i++){
			kill(pid,SIGUSR1);}
		}
		
	}
}
