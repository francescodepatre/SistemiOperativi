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
	int i,s,sock,rval,rval2,portno;
	struct sockaddr_in server;
	struct hostent *hp, *gethostbyname();
	int tsum;
	float avg;
	time_t nsec;
	unsigned short nmil;
	RICHIESTA_MSG request;
	RISPOSTA_MSG answer;
	
	
	
	if(argc!=3){
		exit(-1);
	}
	
	srand(getpid());
	
	sock = socket(AF_INET,SOCK_STREAM,0);
	if(sock<0){perror("sock error");}
	
	server.sin_family = AF_INET;
	hp = gethostbyname(argv[1]);
	
	if(hp==0){
		exit(1);
	}
	
	memcpy(&server.sin_addr,hp->h_addr, hp->h_length);
	portno = atoi(argv[2]);
	server.sin_port = htons(portno);
	
	printf("Connecting...\n");
	
	if(connect(sock,(struct sockaddr *)&server, sizeof(server))<0){
		perror("Connect error");
	}
	
	printf("COnnected\n");
	
	request.req = rand()%100;
	write(sock,&request,sizeof(request));
	
	read(sock,&answer,sizeof(answer));
	
	printf("sent %d, answer is %d\n", request.req, answer.answ);
	close(sock);
	
	
	
	
	return 0;
}
