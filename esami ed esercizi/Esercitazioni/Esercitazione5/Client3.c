#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


typedef struct _RICHIESTA_MSG
{
	int req;
} RICHIESTA_MSG;

typedef struct _RISPOSTA_MSG
{
	int answ;
} RISPOSTA_MSG;


int main(int argc, char *argv[]){
	int i, s, sock, rval, rval2, portno;
	struct sockaddr_in server;
	struct hostent *hp, *gethostbyname();
	int tsum;
	float avg;
	time_t nsec;
	unsigned short nmil; //tipo valore assoluto di short int
	RICHIESTA_MSG request;
	RISPOSTA_MSG answer;
	
	if(argc!=3){
		fprintf(stderr, "Uso: %s hostname portno\n\n", argv[0]);
		exit(-1);	
	}
	
	srand(getpid());
	
	sock = socket(AF_INET, SOCK_STREAM, 0);	

	if(sock <0){
		perror("Opening stream socket");
		exit(1);
	}
	
	server.sin_family = AF_INET;
	hp = gethostbyname(argv[1]);
	if(hp==0){
		fprintf(stderr, "%s: unknown host", argv[1]);
		exit(2);
	}

	memcpy((char *)&server.sin_addr, (char *)hp->h_addr, hp->h_length);
	
	portno = atoi(argv[2]);
	server.sin_port = htons(portno);
	
	printf("Connecting...\n");
	if(connect(sock,(struct sockaddr *)&server, sizeof(server))<0){
	perror("Connectiong stream socket");
	exit(1);
	}
	
	printf("Connected. \n");
	printf("Inserisci il numero\n");
	scanf("%d",&i);
	request.req = i;
	write(sock, &request, sizeof(request));
	int number;
	read(sock,&number, sizeof(number));
	read(sock,&answer,sizeof(answer));
		
	printf("Sent %d - server random is : %d answer is : %d\n" , request.req , number,  answer.answ);
	close(sock);
	exit(0);
}
	
	




















































