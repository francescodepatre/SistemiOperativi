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
	//variabili
	int sock,m=1,msgsock,portno,length;
	struct sockaddr_in server,client;
	
	//creazione socket
	sock = socket(AF_INET,SOCK_STREAM,0);
	if(sock<0){
		perror("sock error");
		exit(-1);
	}
	
	portno = 5555;
	
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(portno);
	length = sizeof(server);
	
	
	//binding
	if(bind(sock, (struct sockaddr *)&server, sizeof(server))<0){
		perror("binding error");
		exit(-1);
	}
	
	
	
	if(getsockname(sock,(struct sockaddr*)&server,(socklen_t*)&length)<0){
		perror("Error");
	}
	
	
	
	//ascolto
	listen(sock,5);
	
	int pid;
	int op1;
	char buffer[256];
	char recive[256];
	do{
		fd_set sock_fd;
	
		FD_ZERO(&sock_fd);
		FD_SET(sock,&sock_fd);
		
		select(sock+1,&sock_fd,0,0,0);
		
		if(FD_ISSET(sock,&sock_fd)){
			msgsock = accept(sock,(struct sockaddr *)&client, (socklen_t *)&length);
			if((pid = fork()) ==0){
				close(sock);
				read(msgsock,recive,sizeof(recive));
				sscanf(recive, "%d", &op1);
				int somma = op1 + 10;
				sprintf(buffer, "%d", somma);
				write(msgsock,buffer, strlen(buffer));
				exit(0);
			}
			else{
				close(msgsock);
			}
						
		
		}
	} while(1);
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	return 0;
}
