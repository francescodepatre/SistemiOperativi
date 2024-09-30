#include<stdio.h>
#include<stdlib.h>  /* serve per rand() e srand() */
#include<time.h> 
#include<unistd.h>
#include<signal.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<fcntl.h>
#include<unistd.h>


void handler(int signo){
printf("E' stato ricevuto un segnale, sono il processo %d, ora termino\n", getpid());
exit(0);
}


int main(int argc, char **argv){
	pid_t pid, pid2;
	int N = atoi(argv[1]);
	struct msg {
    	int mypid;
    	int n;
  	} m1, m2, m3,m4,m5,m6;
  	
	
//preparazione segnale SIGUSR1
sigset_t set,zeromask;
struct sigaction action;
sigemptyset(&zeromask); 
	
sigemptyset(&action.sa_mask);
action.sa_handler = handler;
action.sa_flags =0;
sigemptyset(&set); 
sigaddset(&set, SIGUSR1); 
sigprocmask(SIG_BLOCK, &set , NULL); 

if(sigaction(SIGUSR1, &action, NULL)==-1){
perror("Sigaction error");}



	//GENERIAMO LE PIPE
	int pdpadre[2], pdfiglio1[2], pdfiglio2[2];
	
	if(pipe(pdpadre)<0){
		perror("pipe error");
	}
	if(pipe(pdfiglio1)<0){
		perror("pipe error");
	}
	if(pipe(pdfiglio2)<0){
		perror("pipe error");
	}
	
	
	
	
	if((pid=fork())<0){
		perror("fork error");
	}
	
	if(pid==0){
	//FIGLIO1
	srand(getpid());
	int random = (int) ((N+1) * (rand()/(RAND_MAX+1.0)));
	printf("Sono il figlio1 ed il mio pid vale %d e il numero random è %d\n", getpid(),random);
	m1.n = random;
	m1.mypid = getpid();
	write(pdfiglio2[1], &m1, sizeof(m1));
	write(pdpadre[1], &m1, sizeof(m1));
	read(pdfiglio1[0],&m3,sizeof(m3));
	printf("Sono il figlio1 con pid %d e leggo msg %d e %d\n", getpid(), m3.n,m3.mypid);
	if(m1.n<m3.n){
	printf("Sono figlio1 con numero minore, invio segnale\n");
	kill(m3.mypid,SIGUSR1);
	sigsuspend(&zeromask);
	}
	else{
	sigsuspend(&zeromask);
	}


	}

	
	
	if(pid>0){	
		
	if((pid2=fork())<0){
		perror("fork error");
	}
	
	if(pid2>0){
		//PADRE
		printf("Sono il processo padre ed il mio pid vale %d \n", getpid());
		read(pdpadre[0],&m5,sizeof(m5));
		read(pdpadre[0],&m6,sizeof(m6));
		printf("Sono padre e ho letto m5 %d e %d, m6 %d e %d\n", m5.n,m5.mypid,m6.n,m6.mypid);
		if(m5.n< m6.n){
		waitpid(m6.mypid,NULL,0);
		printf("Il figlio1 ha random minore e vale %d\n",m5.mypid);
		
		kill(m5.mypid,SIGUSR1);
		}
		else{
		waitpid(m5.mypid,NULL,0);
		printf("Il figlio2 ha random minore e vale %d\n",m6.mypid);
		
		kill(m6.mypid,SIGUSR1);
		}
		
	}
	
	else if(pid2==0){
		//FIGLIO2
		srand(getpid());
		int random = (int) ((N+1) * (rand()/(RAND_MAX+1.0)));
		printf("Sono il figlio2 ed il mio pid vale %d e il numero random è %d\n", getpid(),random);
		m2.n = random;
		m2.mypid = getpid();
		write(pdfiglio1[1], &m2, sizeof(m2));
		write(pdpadre[1], &m2, sizeof(m2));
		read(pdfiglio2[0],&m4,sizeof(m4));
		printf("Sono il figlio2 con pid %d e leggo msg %d e %d\n", getpid(), m4.n,m4.mypid);
		if(m2.n<m4.n){
		printf("Sono figlio2 con numero minore, invio segnale ");
		kill(m4.mypid,SIGUSR1);
		sigsuspend(&zeromask);
		}
		else{
		sigsuspend(&zeromask);
		}
	}
	
	
}








}
