#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <malloc.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/stat.h>

#define MAXBUF 100

char request[MAXBUF], answer[MAXBUF], operation;
int op1, op2, result, numSaved = 0, set_log = 0;
int sock1, sock2, length, on, size;
int pid, s, msgsock1, msgsock2, rval, i;
fd_set sock_fdset;
sigset_t set, zeromask;
struct sigaction sig1, sig2;
struct sockaddr_in server1, server2 ,client1, client2;

void handler(int signo){
    switch(signo){
        case SIGUSR1:
            set_log = 1;
            printf("Log visualizzazione operazioni attivato\n");
            break;
        case SIGUSR2:
            set_log = 0;
            printf("Log visualizzazione operazioni disattivato\n");
            break;
        default:
            printf("(Pstore %d) Non dovrebbe entrare qui lo switch di scelta\n", getpid());
    }
}

int main(int argc, char* argv[])
{
    /*Inizializzazione dei segnali*/
    sigemptyset(&set);
    sigemptyset(&zeromask);
    sigaddset(&set,SIGUSR1);
    sigaddset(&set,SIGUSR2);
    
    sigemptyset(&sig1.sa_mask);
    sig1.sa_flags = SA_RESTART;
    sig1.sa_handler = handler;
    if(sigaction(SIGUSR1,&sig1,NULL) < 0){
        perror("Errore nella sigaction 1");
        exit(-1);
    }
    
    sigemptyset(&sig2.sa_mask);
    sig2.sa_flags = SA_RESTART;
    sig2.sa_handler = handler;
    if(sigaction(SIGUSR2,&sig2,NULL) < 0){
        perror("Errore nella sigaction 2");
        exit(-1);
    }
    
    /* Crea la  socket STREAM 1*/
    sock1 = socket(AF_INET,SOCK_STREAM,0);
    if(sock1 < 0){
        perror("opening stream socket");
        exit(1);
    }
    /* Utilizzo della wildcard per accettare le connessioni ricevute da ogni interfaccia di rete del sistema */
    server1.sin_family = AF_INET;
    server1.sin_addr.s_addr = INADDR_ANY;
    server1.sin_port = htons(2000);
    on = 1;
    rval = setsockopt(sock1, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    if (bind(sock1,(struct sockaddr *)&server1,sizeof server1)<0){
        perror("binding stream socket1");
        exit(1);
    }
    length= sizeof(server1);
    if(getsockname(sock1,(struct sockaddr *)&server1,&length)<0){
        perror("getting socket name");
        exit(1);
    }
    
    /* Crea la  socket STREAM 2*/
    sock2 = socket(AF_INET,SOCK_STREAM,0);
    if(sock2 < 0){
        perror("opening stream socket");
        exit(1);
    }
    /* Utilizzo della wildcard per accettare le connessioni ricevute da ogni interfaccia di rete del sistema */
    server2.sin_family = AF_INET;
    server2.sin_addr.s_addr = INADDR_ANY;
    server2.sin_port = htons(2001);
    rval = setsockopt(sock2, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    if (bind(sock2,(struct sockaddr *)&server2,sizeof server2)<0){
        perror("binding stream socket1");
        exit(1);
    }
    length= sizeof(server2);
    if(getsockname(sock2,(struct sockaddr *)&server2,&length)<0){
        perror("getting socket name");
        exit(1);
    }
    
    
    printf("Socket1 port #%d\n",ntohs(server1.sin_port));
    printf("Socket2 port #%d\n",ntohs(server2.sin_port));
    /* Pronto ad accettare connessioni */
    printf("Salve, sono il processo servitore %d .\nSarò un processo di tipo concorrente ma i segnali li riceverò io.\n",getpid());
    listen(sock1,5);
    listen(sock2,5);
    do {
        /* Attesa di una connessione */	
        FD_ZERO(&sock_fdset);
        FD_SET(sock1,&sock_fdset);
        FD_SET(sock2,&sock_fdset);
        if(select(sock2+1,&sock_fdset,NULL,NULL,NULL) > 0){
            if(FD_ISSET(sock1,&sock_fdset)){
                msgsock1 = accept(sock1,(struct sockaddr *)&client1,(socklen_t *)&length);
                if(msgsock1 == -1){
                    perror("accept");
                    exit(-1);
                }
                if((pid =fork()) < 0){
                    perror("Errore nella fork");
                    exit(-1);
                }
                else
                    if(pid == 0){
                        sigprocmask(SIG_BLOCK,&set,NULL);
                        close(sock1); 
                        if(read(msgsock1,&request,sizeof(request)) < 0)
                            perror("Errore nella lettura del messaggio inviato dal client");
                        sscanf(request,"%d %d", &op1, &op2);
                        result = op1+op2;
                        sprintf(answer,"%d + %d = %d\n",op1,op2,result);
                        if(set_log)
                            printf("%s",answer);
                        write(msgsock1,answer,strlen(answer));
                        
                        close(msgsock1);
                        exit(0);     
                    }
                    else
                        close(msgsock1);
            }
            if(FD_ISSET(sock2,&sock_fdset)){
                msgsock2 = accept(sock2,(struct sockaddr *)&client2,(socklen_t *)&length);
                if(msgsock2 == -1){
                    perror("accept");
                    exit(-1);
                }
                if((pid =fork()) < 0){
                    perror("Errore nella fork");
                    exit(-1);
                }
                else
                    if(pid == 0){
                        sigprocmask(SIG_BLOCK,&set,NULL);
                        close(sock2); 
                        if(read(msgsock2,&request,sizeof(request)) < 0)
                            perror("Errore nella lettura del messaggio inviato dal client");
                        sscanf(request,"%d %d", &op1, &op2);
                        result = op1*op2;
                        sprintf(answer,"%d * %d = %d\n",op1,op2,result);
                        if(set_log)
                            printf("%s",answer);
                        write(msgsock2,answer,strlen(answer));
                        
                        close(msgsock2);
                        exit(0);     
                    }
                    else
                        close(msgsock2);
            }
        }
    }
    while(1);
    close(sock1); close(sock2);
    exit(0);
}
