#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <string.h>

void error(char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char send_buffer[256];
    char rec_buffer[256]="";

    if (argc < 3) {
       fprintf(stderr,"uso %s nomehost porta\n", argv[0]);
       exit(0);
    }
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERRORE in apertura");
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERRORE, l'host non esiste\n");
        exit(0);
    }
    
    serv_addr.sin_family = AF_INET;
    memcpy((char *)&serv_addr.sin_addr,(char *)server->h_addr,server->h_length);
   
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) 
        error("ERRORE di connessione");
    printf("(CLIENT) Scrivere un messaggio: ");
   
    fgets(send_buffer,255,stdin);
    n = write(sockfd,send_buffer,strlen(send_buffer));
    if (n < 0) 
         error("ERRORE in scrittura sulla socket");
    
    n = read(sockfd,rec_buffer,255);
    if (n < 0) 
         error("ERRORE in lettura sulla socket");
    printf("(CLIENT) Ecco il messaggio ricevuto dal server: %s\n",rec_buffer);

    close(sockfd);

    return 0;
}
