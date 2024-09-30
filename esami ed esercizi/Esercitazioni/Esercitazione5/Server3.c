#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct _RICHIESTA_MSG
{
	int req;
} RICHIESTA_MSG;

typedef struct _RISPOSTA_MSG
{
	int answ;
} RISPOSTA_MSG;


int main( int argc, char* argv[]){
	int sock, length, portno;
	struct sockaddr_in server, client;
	int pid,s,msgsock,rval,rval2,i;
	struct hostent *hp,*gethostbyname();
	RICHIESTA_MSG request;
	RISPOSTA_MSG answer;

	//CONTROLLO GLI ARGOMENTI
	if(argc!=2){
	printf("Usare: %s <port_number> \n" , argv[0]);
	exit(-1);
	}
		
	//CREA LA SOCKET STREAM
	sock = socket(AF_INET,SOCK_STREAM,0);
	if(sock<0){
		perror("Opening stream socket");
		exit(1);
	}

	portno = atoi(argv[1]);
	
	server.sin_family = AF_INET;
	//Uso la wildcard INADDR_ANY per acettare conessioni rievute da qualunque interfaccia di rete del sistema
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(portno);
	
	if(bind(sock,(struct sockaddr *)&server, sizeof(server))<0){
		perror("Binding error");	
		exit(1);
	}

	length = sizeof(server);
	if(getsockname(sock,(struct sockaddr *)&server, &length)<0){
		perror("Getting socket name");	
		exit(1);
	}
	
	printf("Socket port #%d\n" , ntohs(server.sin_port));
	
	//Pronto ad accettare connessioni
	
	listen(sock,2);
	
	do{
	//attesa di una connessione
	
	msgsock = accept(sock,(struct sockaddr *)&client, (socklen_t *)&length);
	
	if(msgsock ==-1){
		perror("accept");}
	else
	{
		printf("Connection from %s, port %d\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));
	//server corrente
	if((pid =fork()) ==0){
		close(sock);
		read(msgsock, &request, sizeof(request));
		
		//Esecuzione del servizio
		i = rand()%100;
		write(msgsock, &i, sizeof(i));
		answer.answ = request.req + i;
		write(msgsock, &answer, sizeof(answer));
		
		close(msgsock);
		exit(0);
	}
		
	else {
		if(pid ==-1){ perror("fork error"); exit(-1);}
		else{
			//ok il padre torna in accept
		close(msgsock);
	}
	}
}	
}
	
	
while(1);
exit(0);
}















































