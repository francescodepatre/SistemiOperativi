#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct _RICHIESTA_MSG 
{
  int req;
} RICHIESTA_MSG;

typedef struct _RISPOSTA_MSG 
{
  int answ;
} RISPOSTA_MSG;


main(int argc, char* argv[])
{
  int	sock,length,portno;
  struct  sockaddr_in 	server,client;
  int	pid,s,msgsock,rval,rval2,i;
  struct	hostent	*hp,*gethostbyname();
  RICHIESTA_MSG request;
  RISPOSTA_MSG  answer;
  
  if (argc !=2) {
    printf("Usage: %s <port_number>\n", argv[0]);
    exit(-1);
  }

  /* Crea la  socket STREAM */
  sock=	socket(AF_INET,SOCK_STREAM,0);
  if(sock<0)
  {	
    perror("opening stream socket");
    exit(1);
  }
  
  portno = atoi(argv[1]);
  
  /* Utilizzo della wildcard per accettare le connessioni ricevute da ogni interfaccia di rete del sistema */
  server.sin_family = 	AF_INET;
  server.sin_addr.s_addr= INADDR_ANY;
  server.sin_port = htons(portno);
  if (bind(sock,(struct sockaddr *)&server,sizeof server)<0)
  {
    perror("binding stream socket");
    exit(1);
  }
  
  
  length= sizeof(server);
  if(getsockname(sock,(struct sockaddr *)&server,&length)<0)
  {
    perror("getting socket name");
    exit(1);
  }
  
  printf("Socket port #%d\n",ntohs(server.sin_port));
  
  /* Pronto ad accettare connessioni */
  
  listen(sock,2);
  
  do {
    /* Attesa di una connessione */	 
      
    msgsock= accept(sock,(struct sockaddr *)&client,(socklen_t *)&length);
      
    if(msgsock ==-1)
      perror("accept");
    else
    { 
      printf("Connection from %s, port %d\n", 
             inet_ntoa(client.sin_addr), ntohs(client.sin_port));
	 
      if((pid = fork())== 0)
      {
        close(sock);
        read(msgsock,&request,sizeof(request));

        /* Esecuzione del servizio */
        answer.answ = request.req + (rand()%100);
        write(msgsock,&answer,sizeof(answer));
        close(msgsock);
        exit(0);     
      }
      else
      {
        if(pid == -1)	/* Errore nella fork */
        {
          perror("Error on fork: ");
          exit(-1);
        }
        else
	{	
          /* OK, il padre torna in accept */
          close(msgsock);
        }
      }	  
    }
      
  }
  while(1);
  exit(0);
}
