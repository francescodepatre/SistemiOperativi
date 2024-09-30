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
printf("E' stato ricevuto un segnale, sono il processo %d\n", getpid());
}



int main(){
	//determiniamo la struttura dei messaggi
	struct msg {
		int mypid;
    		int n;
  	} m1, m2, m3,m4,m5;
	pid_t pid, pid2;
	
	
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


	//apriamo le pipe di comunicazione
	int pdpadre[2], pdfiglio1[2], pdfiglio2[2];
	
	if(pipe(pdfiglio2)<0){
	perror("pip error");
	}

	if(pipe(pdfiglio1)<0){
		perror("pip error");
	}

	if(pipe(pdpadre)<0){
		perror("pip error");
	}
	
	//creazione figli
	if((pid = fork())<0){
		perror("fork error");
	}
	
if(pid==0){
	//figlio1
	printf("Sono il figlio1 e il mio pid vale %d\n",getpid());
	read(pdfiglio1[0], &m3, sizeof(m3));
		if(m3.n==0){
		sigsuspend(&zeromask);
		sleep(3);
		printf("Ho aspettato tre sec ora termino\n");
		exit(0);
		}
		else{
		kill(m3.mypid,SIGUSR1);
		system("ps");
	printf("Sono il figlio1 e il messaggio recita %d e %d pid\n", m3.n, m3.mypid);
}
}


if(pid>0){
		
	if((pid2=fork())<0){
		perror("fork error");
	}
	
	if(pid2>0){
	//padre
		printf("Sono il padre e il mio pid vale %d\n", getpid());
		
		srand(getpid());
		int random = rand()%100;
		m1.n = random;
		m1.mypid= pid2;
		m4.mypid = pid;
		m4.n = random;
		m5.n =0;
		m5.mypid=0;
		if(random%2==0){
			write(pdfiglio1[1], &m1, sizeof(m1));
			write(pdfiglio2[1],&m5,sizeof(m5));
		}
		else{
			write(pdfiglio2[1],&m4,sizeof(m4));
			write(pdfiglio1[1], &m5, sizeof(m5));
		}
		sleep(10);
		exit(0);
	}
	
	
	
	
	else{
	//figlio2
		printf("Sono il figlio2 e il mio pid vale %d\n",getpid());
		read(pdfiglio2[0], &m2,sizeof(m2));
		if(m2.n==0){
		sigsuspend(&zeromask);
		sleep(3);
		printf("Ho aspettato tre sec ora termino\n");
		exit(0);
		}
		else{
		kill(m2.mypid,SIGUSR1);
		system("ps");
		printf("Sono il figlio2 e il messaggio recita %d e %d pid\n", m2.n, m2.mypid);
		}
	}
	
	
	
	
}	
}
