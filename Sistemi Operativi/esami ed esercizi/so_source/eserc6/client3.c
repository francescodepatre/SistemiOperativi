#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct _RICHIESTA_MSG 
{
  int req;
} RICHIESTA_MSG;

typedef struct _RISPOSTA_MSG 
{
  int answ; 
} RISPOSTA_MSG;

main(int argc,char* argv[])
{
  int	i,s,sock,rval,rval2,portno;
  struct 	sockaddr_in 	server;
  struct	hostent	*hp,*gethostbyname();
  int tsum;
  float avg;
  time_t nsec;
  unsigned short nmil;
  RICHIESTA_MSG request;
  RISPOSTA_MSG  answer;
  
  
  if(argc != 3)
  {
    fprintf(stderr,"Uso: %s hostname portno\n\n",argv[0]);
    exit(-1);
  }
  
  srand(getpid());
  
  /* Crea una  socket di tipo STREAM per il dominio TCP/IP */
  sock=	socket(AF_INET,SOCK_STREAM,0);
  
  if(sock<0)
  {
    perror("opening stream socket");
    exit(1);
  }
  
  
  /* Ottiene l'indirizzo del server */
  server.sin_family = AF_INET;
  hp= gethostbyname(argv[1]);
  
  if(hp==0)
  {
    fprintf(stderr,"%s: unknown host",argv[1]);
    exit(2);
  }
  
  
  memcpy((char *)&server.sin_addr, (char *)hp->h_addr, hp->h_length);
  
  /* La porta e' sulla linea di comando */
  portno = atoi(argv[2]);
  server.sin_port= htons(portno);
  
  /* Tenta di realizzare la connessione */
  printf("Connecting...\n");
  if(connect(sock,(struct sockaddr *)&server,sizeof server) <0)
  {
    perror("connecting stream socket");
    exit(1);
  }
  
  printf("Connected.\n");
  
  request.req = rand() %100;
  write(sock,&request,sizeof(request));
  
  read(sock,&answer,sizeof(answer));
  
  printf("Sent %d - server answer is : %d\n",request.req,answer.answ);
  
  close(sock);
  
  exit(0);
}
