#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <sys/socket.h>
#include<fcntl.h>
#include<netdb.h>
#include<netinet/in.h>
#include<string.h>



void handler(int signo){
	printf("Segnale ricevuto\n");
	//body handler
	
}


int main(int argc, char **argv){
	//variabili socket
	int sock, msgsock, lunghezza,portno = 9999;
	char buffer[256];
	struct sockaddr_in server, client;
	struct sigaction sig;

//per la seconda socket il procedimento è analogo

	//creazione prima socket
	sock = socket(AF_INET,SOCK_STREAM,0);
	if(sock<0){
		perror("Sock error");
	}
	
	
	//definizione struttura socket
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(portno);
	lunghezza = sizeof(server);
	
	//assegno nome alla socket
	if(bind(sock,(struct sockaddr*)&server,sizeof(server))<0){
		perror("Error");
	}
	
	if(getsockname(sock,(struct sockaddr*)&server,(socklen_t*)&lunghezza)<0){
		perror("Error");
	}
	
	//piping padre e figlio nel caso servisse comunicazione
	int pip[2];
	if(pipe(pip)<0){
		perror("Pipe error");
	}
	
	
	
	//ascolto
	listen(sock,5);
	
	
	//variabili per socket concorrente
	int pid,pid2;
	
	
	
	
	//segnali sigusr1 al server
	sigemptyset(&sig.sa_mask);
	sig.sa_handler = handler; 
	sig.sa_flags =SA_RESTART;


	sigaction(SIGUSR1, &sig, NULL);
	
	
	//ciclo do while
	do{
		
		//se figlio deve comunicare col padre
		fd_set readfd;
	
		FD_ZERO(&readfd);
		FD_SET(pip[0],&readfd);
		timeout.tv_sec =0;
		timeout.tv_usec =0;
		if(select(pip[0]+1,&readfd,NULL,NULL,&timeout)>0){
			read(pip[0],&messaggio,sizeof(messaggio));
			
		}
		
		
		//select tra due client
		fd_set sock_fd;
	
		FD_ZERO(&sock_fd);
		FD_SET(sock,&sock_fd);
		FD_SET(sock2,&sock_fd);
	
		//applico la select
		//trovo il massimo
		if(sock>sock2){
			select(sock+1,&sock_fd,0,0,0);
		}
		else{
			select(sock2+1,&sock_fd,0,0,0);
		}
		
		//se vince il primo client faccio questo, altrimenti procedimento analogo per il secondo
		if(FD_ISSET(sockfd,&sock_fd)){
			msgsock = accept(sock,(struct sockaddr *)&client, (socklen_t *)&lung);
			if((pid = fork())==0){
				close(sock);
			 //body del server
			}
			else{
				close(msgsock);
			}
		}
		
			
	}while(1);
	
	
	
	
//Finita la programmazione server client

//numero random
srand(getpid());
num = rand()%(100 +1) //da zero a 100

//gestione segnali senza server
struct sigaction act;
sigset_t sigmask, zeromask;
sigemptyset(&zeromask);
sigemptyset(&sigmask);
sigaddset(&sigmask,SIGUSR1);

sigprocmask(SIG_BLOCK,&sigmask,NULL);
act.sa_handler = handler;
sigemptyset(&act.sa_mask);
act.sa_flags =0;
sigaction(SIGUSR1,&act,NULL);
//ricezione ed invio
sigsuspend(&zeromask);
kill(pid,SIGUSR1);
	
	
	
	
	
	
	
	
	
	
	
	
	return 0;
}
