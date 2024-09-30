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

int main(int argc, char **argv){


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



	//variabili socket
	int sockfd,portno=1111,msgsock,lenght,portno2=9999,sockfd2,msgsock2,lenght2;
	char buffer[256];
	int op1,op2;
	struct sockaddr_in server1,server2,client;
	
	//creazione prima socket
	sockfd = socket(AF_INET,SOCK_STREAM,0);
	if(sockfd<0){
		perror("Sock error");
	}
	
	//definizione struttura socket
	server1.sin_family = AF_INET;
	server1.sin_addr.s_addr = INADDR_ANY;
	server1.sin_port = htons(portno);
	lenght = sizeof(server1);
	
	//assegno nome alla socket
	if(bind(sockfd,(struct sockaddr*)&server1,sizeof(server1))<0){
		perror("Error");
	}
	
	if(getsockname(sockfd,(struct sockaddr*)&server1,(socklen_t*)&lenght)<0){
		perror("Error");
	}
	
	//ascolto
	listen(sockfd,5);
	
	
		//creazione seconda socket
	sockfd2 = socket(AF_INET,SOCK_STREAM,0);
	if(sockfd2<0){
		perror("Sock error");
	}
	
	//definizione struttura socket
	server2.sin_family = AF_INET;
	server2.sin_addr.s_addr = INADDR_ANY;
	server2.sin_port = htons(portno2);
	lenght2 = sizeof(server2);
	
	//assegno nome alla socket
	if(bind(sockfd2,(struct sockaddr*)&server2,sizeof(server2))<0){
		perror("Error");
	}
	
	if(getsockname(sockfd2,(struct sockaddr*)&server2,(socklen_t*)&lenght2)<0){
		perror("Error");
	}
	
	//ascolto
	listen(sockfd2,5);
	

	int pid1,pid2;
	char recive[256];
	
	
	do{
	//SELECT
	fd_set sock_fd;
	
	FD_ZERO(&sock_fd);
	FD_SET(sockfd,&sock_fd);
	FD_SET(sockfd2,&sock_fd);
	
	//applico la select
	//trovo il massimo
	if(sockfd>sockfd2){
		select(sockfd+1,&sock_fd,0,0,0);
	}
	else{
		select(sockfd2+1,&sock_fd,0,0,0);
	}
	
	
	
	
	if(FD_ISSET(sockfd,&sock_fd)){	
	msgsock = accept(sockfd,(struct sockaddr*)&client,(socklen_t*)&lenght);
	if((pid1=fork())==0){
		close(sockfd);
		read(msgsock,recive,sizeof(recive));
		sscanf(recive,"%d%d",&op1,&op2);
		int somma = op1+op2;
		sprintf(buffer,"%d",somma);
		printf("Somma = %d\n", somma);
		write(msgsock,buffer,strlen(buffer));
		close(msgsock);
		exit(0);
	}
	else{close(msgsock);}

	}
	
	if(FD_ISSET(sockfd2,&sock_fd)){
	msgsock2 = accept(sockfd2,(struct sockaddr*)&client,(socklen_t*)&lenght);
	if((pid2=fork())==0){
		close(sockfd2);
		read(msgsock2,recive,sizeof(recive));
		sscanf(recive,"%d %d",&op1,&op2);
		int prodotto = op1*op2;
		sprintf(buffer,"%d",prodotto);
		printf("Prodotto = %d\n", prodotto);
		write(msgsock2,buffer,strlen(buffer));
		exit(0);
	}
	else{close(msgsock2);}
}}while(1);
}
