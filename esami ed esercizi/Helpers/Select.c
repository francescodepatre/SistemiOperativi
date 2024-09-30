#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<netdb.h>



int main(int argc, char **argv){
	//variabili prima socket e variabili generali
	int sock, portno;
	socklen_t length;
	struct sockaddr_in server, client;
	int pid, msgsock;
	struct hostent *gethostbyname();
	char buf[256];
	
	//variabili seconda socket
	int sock2;
	socklen_t length2;
	int msgsock2;
	struct sockaddr_in client2;
	
	//creazione socket
	sock = socket(AF_INET,SOCK_STREAM,0);
	if(sock<0){
		perror("Sock error");
		exit(-1);
	}
	
	portno = 9999;
	
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(portno);
	
	//creazione seconda socket
	sock2 = socket(AF_INET,SOCK_STREAM,0);
	if(sock2<0){
		perror("Sock error");
		exit(-1);
	}
	
	
		
	//bind
	if(bind(sock, (struct sockaddr *)&server, sizeof(server))<0){
		perror("bind error");
		exit(-1);
	}
	
	length = sizeof(server);
	
	

		
	//nome del server
	if(getsockname(sock, (struct sockaddr *)&server, &length)<0){
		perror("name error");
		exit(-1);
	}
	
	//nome del server
	if(getsockname(sock2, (struct sockaddr *)&server, &length)<0){
		perror("name error");
		exit(-1);
	}
	
	printf("Socket port %d\n", ntohs(server.sin_port));
	//accettiamo le connessioni
	listen(sock,2);
	listen(sock2,2);
	//accept
	
	do {
	//SELECT
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
	
	
	if(FD_ISSET(sock,&sock_fd)){	
		msgsock = accept(sock,(struct sockaddr *)&client, (socklen_t *)&length);
		
		if(msgsock<0){
			perror("accept error");
			exit(-1);
		}
		else{
			printf("Connection form %s, port %d\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));
		
			if(pid =fork()==0){
				close(sock);
				read(msgsock, &buf,sizeof(buf));
				printf("E' arrivato %s e ho restituito %s",buf,buf);
				close(msgsock);
				msgsock2 = accept(sock2,(struct sockaddr *)&client2, (socklen_t *)&length);
				write(msgsock2,&buf,strlen(buf));
				close(msgsock2);
				
				exit(0);
			}
			else if(pid ==-1){
				perror("fork error");
				exit(-1);
			}
}
		}
	

		
	} while(1);
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	return 0;
}
