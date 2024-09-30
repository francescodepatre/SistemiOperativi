#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<netdb.h>

typedef struct _RICHIESTA_MSG{
	int req;
} RICHIESTA_MSG;

typedef struct _RISPOSTA_MSG{
	int answ;
} RISPOSTA_MSG;

int main(int argc, char **argv){
	
	int sock, portno;
	socklen_t length;
	struct sockaddr_in server, client;
	int pid, msgsock;
	struct hostent *gethostbyname();
	RICHIESTA_MSG request;
	RISPOSTA_MSG answer;
	
	//creazione socket
	sock = socket(AF_INET,SOCK_STREAM,0);
	if(sock<0){
		perror("Sock error");
		exit(-1);
	}
	
	portno = atoi(argv[1]);
	
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(portno);
	
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
	printf("Socket port %d\n", ntohs(server.sin_port));
	//accettiamo le connessioni
	listen(sock,2);
	
	//accept
	
	do {
		msgsock = accept(sock,(struct sockaddr *)&client, (socklen_t *)&length);
		if(msgsock<0){
			perror("accept error");
			exit(-1);
		}
		else{
			printf("Connection form %s, port %d\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));
		
			if(pid =fork()==0){
				close(sock);
				read(msgsock, &request,sizeof(request));
				answer.answ = request.req + (rand()%100);
				printf("E' arrivato %d e ho restituito %d", request.req, answer.answ);
				write(msgsock,&answer,sizeof(answer));
				close(msgsock);
				exit(0);
			}
			else if(pid ==-1){
				perror("fork error");
				exit(-1);
			}
			else{
				close(msgsock);
			}
		
		
		}
		
		
		
	} while(1);
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	return 0;
}
