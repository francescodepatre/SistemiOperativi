#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>


main(int argc, char *argv[])
{
  char buf[26];
  time_t ticks;
  int sock,length,msgsock,rval,portno;
  struct sockaddr_in server;	
  struct hostent *hp;


  if (argc !=2) {
    printf("Usage: %s <port_number>\n", argv[0]);
    exit(-1);
  }

  printf("\n*-----------------------------------------------*\n");
  printf("*                                               *\n");
  printf("*         SERVER data e ora correnti            *\n");
  printf("*                                               *\n");
  printf("* ^c per terminare!                             *\n");
  printf("*                                               *\n");
  printf("*-----------------------------------------------*\n\n");
  
  sock=	socket(AF_INET,SOCK_STREAM,0);    /* Crea la  socket STREAM */
  if(sock<0)
  {	
    perror("opening stream socket");
    exit(1);
  }

  portno = atoi(argv[1]);

  server.sin_family = AF_INET;         /* Utilizzo della wildcard per*/ 
  server.sin_addr.s_addr= INADDR_ANY;  /* accettare le connessioni ricevute su ogni interfaccia di rete (ovvero qualunque indirizzo IP del nodo del server )*/
  server.sin_port = htons(portno);
  if (bind(sock,(struct sockaddr *)&server,sizeof server)<0)
  {
    perror("binding stream socket");
    exit(1);
  }


  length= sizeof server;
  if(getsockname(sock,(struct sockaddr *)&server,&length)<0)
  {
    perror("getting socket name");
    exit(1);
  }

  printf("Socket port #%d\n\n",ntohs(server.sin_port));

  
  listen(sock,2); /* Pronto ad accettare connessioni */

  do { 
    /* Attesa di una connessione */   	 	  
    msgsock= accept(sock,(struct sockaddr *)0,(int *)0);

    if(msgsock ==-1)
      perror("accept");
    else
    {	    
      ticks = time(NULL);        /*calcola data e ora corrente*/
      strcpy(buf,ctime(&ticks)); /*la converte in stringa e la memorizza in buf[26]*/

      /* Invio dell'informazione */
      if((rval = write(msgsock,buf,sizeof buf))<0)  
        perror("writing on stream socket");
      printf("%d byte scritti\n",rval);	      
    } 
    close (msgsock);
  }
  while(1);

  exit(0);
}
