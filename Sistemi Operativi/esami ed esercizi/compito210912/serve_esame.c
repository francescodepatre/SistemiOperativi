#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <signal.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <malloc.h>
#include <fcntl.h>

#define PORT 2000
#define MAXBUF 100

int	sock, length, portno, on, piped[2];
int pid,s,msgsock,rval,rval2,i;
int q, M = 1, dataClient, risultato;
struct  sockaddr_in server,client;
sigset_t set, zeromask;
struct sigaction sig1,sig2;
char request[MAXBUF], answer[MAXBUF];

void handler(int signo){
    switch(signo){
        case SIGUSR1: /*Incremento di 1 la variabile q*/
            q++;
            printf("(SIGUSR1) Vecchio q = %d, Nuovo q = %d\n",q-1,q);
            break;
        case SIGUSR2: /*Decremento di 1 la variabile q*/
            q--;
            printf("(SIGUSR2) Vecchio q = %d, Nuovo q = %d\n",q+1,q);
            break;
        default:
            printf("Non dovrebbe mai entrare qui\n");
    }
}

int main(int argc, char* argv[])
{    
    if(argc != 2){
        perror("Numero di argomenti di invocazione dicerso da 2");
        exit(-1);
    }
    q = atoi(argv[1]);
    if(q < 0){
        printf("Valore dell'argomento passato da terminale < 0, verra' messo di default a 0\n");
        q=0;
    }
    else
        if(q > 100){
            printf("Valore dell'argomento passato da terminale > 100, verra' messo di default a 100\n");
            q=100;
        }
    printf("Valore dell'argomento d'invocazione: %d\n",q);
    
    /*Inizializzazione dei segnali*/
    sigemptyset(&set);
    sigemptyset(&zeromask);
    sigaddset(&set,SIGUSR1);
    sigaddset(&set,SIGUSR2);
    
    sigemptyset(&sig1.sa_mask);
    sig1.sa_handler = handler;
    sig1.sa_flags = SA_RESTART;
    if(sigaction(SIGUSR1,&sig1,NULL) < 0){
        perror("Errore nella sigaction 1");
        exit(-1);
    }
    
    sigemptyset(&sig2.sa_mask);
    sig2.sa_handler = handler;
    sig2.sa_flags = SA_RESTART;
    if(sigaction(SIGUSR2,&sig2,NULL) < 0){
        perror("Errore nella sigaction 2");
        exit(-1);
    }
    
    /*Inizializzazione di eventuali pipe*/
    if(pipe(piped) < 0){
        perror("Errore nella creazione della pipe");
        exit(-1);
    }
    write(piped[1],&M,sizeof(M));
    
    /* Crea la  socket STREAM */
    sock = socket(AF_INET,SOCK_STREAM,0);
    if(sock<0){
        perror("opening stream socket");
        exit(1);
    }
    
    portno = PORT;
    /* Utilizzo della wildcard per accettare le connessioni ricevute da ogni interfaccia di rete del sistema */
    server.sin_family = AF_INET;
    server.sin_addr.s_addr= INADDR_ANY;
    server.sin_port = htons(portno);
    on = 1;
    rval = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    if (bind(sock,(struct sockaddr *)&server,sizeof server)<0){
        perror("binding stream socket");
        exit(1);
    }
    length= sizeof(server);
    if(getsockname(sock,(struct sockaddr *)&server,&length)<0){
        perror("getting socket name");
        exit(1);
    }
    printf("Socket port #%d\n",ntohs(server.sin_port));
    /* Pronto ad accettare connessioni */
    printf("%d e' il PID dove i segnali possono essere ricevuti\n",getpid());
    listen(sock,5);
    do {
        /* Attesa di una connessione */	
        msgsock= accept(sock,(struct sockaddr *)&client,(socklen_t *)&length);
        if(msgsock ==-1)
            perror("accept");
        else{             
            if((pid = fork())== 0){ /*Esecuzione della richiesta mediante un figlio*/
                sigprocmask(SIG_BLOCK,&zeromask,NULL);
                close(sock);
		if(read(piped[0],&M,sizeof(M)) < 0){
                    perror("Errore nella lettura del dato dal Cliente");
                    exit(-1);
                }
                if(read(msgsock,&request,sizeof(request)) < 0){
                    perror("Errore nella lettura del dato dal Cliente");
                    exit(-1);
                }
                dataClient = atoi(request);
                printf("Il dato ricevuto dal cliente è: %d\n",dataClient);
                if((dataClient%100) == 0 && dataClient != 0){  /*Se il server ha inviato un valore divisibile per 100*/
                    M = dataClient/100;
                    printf("Il valore ricevuto e' divisibile per 100. Questo e' il nuovo valore della variabile M: %d\n",M);
                }
                printf("Riepilogo. M = %d, dataClient = %d, q = %d\n",M,dataClient,q);
                /* Esecuzione del servizio */
                risultato = (M*dataClient)+q;
                sprintf(answer,"\nM*dataClient+q = %d*%d+%d = %d\n",M,dataClient,q,risultato);
                write(msgsock,answer,strlen(answer));
		write(piped[1],&M,sizeof(M));
                exit(0);
            }
            else{
                if(pid == -1){	/* Errore nella fork */
                    perror("Error on fork: ");
                    exit(-1);
                }
                else{ /* OK, il padre torna in accept */
                    close(msgsock);
                }
            }	  
        }
        
    }
    while(1);
    close(sock);
    exit(0);
}
