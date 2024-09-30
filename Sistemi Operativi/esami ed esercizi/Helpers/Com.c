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
	int sock,msgsock1,msgsock2,lung,portno;
	struct sockaddr_in server,client1,client2;
	
	char buffer[256];
	
	if(argc!=2){
		printf("Servono due argomenti\n");
		exit(-1);
	}
	
	portno = atoi(argv[1]);
	sock = socket(AF_INET,SOCK_STREAM,0);
	if(sock<0){
		perror("sock error");
		exit(-1);
	}
	
	server.sin_family = AF_INET;
	server.sin_addr.s_addr= INADDR_ANY;
	server.sin_port = htons(portno);
	
	lung = sizeof(server);
	
	if(bind(sock, (struct sockaddr *)&server, sizeof(server))<0){
		perror("Bind error");
		exit(-1);
	}
	
	if(getsockname(sock, (struct sockaddr *)&server,(socklen_t *) &lung)<0){
		perror("Error");
		exit(-1);
	}
	
	listen(sock,5);
	
	printf("Socket port %d\n", ntohs(server.sin_port));
	
	do{
	int pid;
	if(pid=fork()==0){
		
	
	
	
		msgsock1 = accept(sock,(struct sockaddr *)&client1, (socklen_t *)&lung);
		msgsock2 = accept(sock,(struct sockaddr *)&client2, (socklen_t *)&lung);
		close(sock);
		read(msgsock1,buffer,sizeof(buffer));
		write(msgsock2,"Il messaggio ricevuto è ", 25);
		write(msgsock2, buffer, strlen(buffer));
		close(msgsock1);
		close(msgsock2);
		exit(0);
	
	
	}
	}while(1);
	
	
	
	return 0;
	
}
