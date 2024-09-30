#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

void error(char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
     int sockfd, newsockfd, portno;
     socklen_t clilen;
     char buffer[256]="";
     struct sockaddr_in serv_addr, cli_addr;
     int n;
     if (argc < 2) {
         fprintf(stderr,"ERRORE, nessuna porta specificata\n");
         exit(1);
     }
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) 
        error("ERRORE DI APERTURA DELLA SOCKET");
    
     portno = atoi(argv[1]);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) 
              error("ERRORE DI BINDING");
     listen(sockfd,5);
     clilen = sizeof(cli_addr);
     newsockfd = accept(sockfd, 
                 (struct sockaddr *) &cli_addr, 
                 (socklen_t *)&clilen);
     if (newsockfd < 0) 
          error("ERRORE DI ACCEPT");
    
     n = read(newsockfd,buffer,255);
     if (n < 0) error("ERRORE in lettura dalla socket");
     printf("(SERVER) Ecco il messaggio ricevuto dal client: %s\n",buffer);
     n = write(newsockfd,"MESSAGGIO RICEVUTO, FINE COMUNICAZIONE",38);
     if (n < 0) error("ERRORE in scrittura sulla socket");

     close(sockfd);
     close(newsockfd);

     return 0; 
}
