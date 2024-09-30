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

int q;

void handler(int signo){
	printf("Segnale ricevuto stmapo q %d\n",q);
	q+=1;
}



int main(int argc, char **argv){

struct sigaction sig;




	int sock, msgsock, lung;
	struct sockaddr_in server, client;
	int portno;
	int dato,m=1,risposta;
	portno = atoi(argv[1]);
	q = atoi(argv[2]);
	sock = socket(AF_INET,SOCK_STREAM,0);
	char buffer[256];
	char response[256];
	server.sin_port = htons(portno);
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	struct timeval timeout;
	lung = sizeof(server);
	
	if(bind(sock, (struct sockaddr *)&server, sizeof(server))<0){
		perror("Bind error");
		exit(-1);
	}
	
	if(getsockname(sock, (struct sockaddr *)&server,(socklen_t *) &lung)<0){
		perror("Error");
		exit(-1);
	}
	int pip[2];
	if(pipe(pip)<0){
		perror("pip error");
		exit(-1);
	}
sigemptyset(&sig.sa_mask);
sig.sa_handler = handler; 
sig.sa_flags =SA_RESTART;


sigaction(SIGUSR1, &sig, NULL);
	printf("Il mio pid è %d\n", getpid());
	listen(sock,5);
	int pid;
	do{
		msgsock = accept(sock,(struct sockaddr *)&client, (socklen_t *)&lung);
		fd_set readfd;
	
		FD_ZERO(&readfd);
		FD_SET(pip[0],&readfd);
		timeout.tv_sec =0;
		timeout.tv_usec =0;
		if(select(pip[0]+1,&readfd,NULL,NULL,&timeout)>0){
			read(pip[0],&m,sizeof(m));
			
		}
			
		


		if((pid = fork()) ==0){

sigemptyset(&sig.sa_mask);
sig.sa_handler = handler; 
sig.sa_flags =SA_RESTART;


sigaction(SIGUSR1, &sig, NULL);
			close(sock);
			printf("Il mio pid riulta %d \n",getpid());
			read(msgsock,buffer,sizeof(buffer));
			sscanf(buffer,"%d",&dato);
			printf("IL DAto %d \n",dato);
			if((dato%100)==0){
				m = dato/100;
				write(pip[1],&m,sizeof(m));
			}
			risposta = m*dato +q;
			sprintf(response, "%d", risposta);
			printf("Risposta = %d\n", risposta);
			write(msgsock, response, strlen(response));
			exit(0);
		}
		else{
			close(msgsock);
		}
		
	}while(1);
	
	return 0;
}
