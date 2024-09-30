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

#define MAXBUF 25
#define PORT 8701

int pid, sock, length, portno, piped[2], on;
int s, msgsock, rval, rval2, i, pidstore, memoria[10];
char message[MAXBUF];
struct  sockaddr_in  server,client;
sigset_t set, zeromask;
fd_set rpipe_fds;
struct sigaction act1, act2;

void bodyStore(){
    int indice, valore;
    char word[5];
    do{
        if(read(piped[0],message,sizeof(message)) < 0){
            perror("Errore nella lettura dalla pipe");
            exit(-1);
        }
        sscanf(message,"%s %d %d",word,&indice,&valore);
        printf("(Pstore %d): %s %d %d\n", getpid(), word, indice, valore);
        if(indice < 0 || indice >= 10 || strcmp(word,"STORE"))
            printf("Valore dell'indice non consentito (oppure cast dell'azione non consentita), il dato non verrà salvato\n");
        if(strcmp(word,"STORE") == 0 && indice >= 0 && indice < 10)
            memoria[indice] = valore;
    }while(1);
    return;
}

void storeIn(int signo){
    int j;
    printf("(Pstore %d) Ricevuto una SIGUSR1.\nQuesta e' la tabella dei dati salvati in 'Pstore':\n", getpid());
    for(j=0;j<10;j++)
        printf("Indice: %d Valore: %d\n", j, memoria[j]);
}

void clearAll(int signo){
    int j;
    printf("(Pstore %d) Ricevuto una SIGUSR2.\nLa tabella dei dati salvati in 'Pstore' si azzerera'\n", getpid());
    for(j=0;j<10;j++)
        memoria[j] = 0;
}

int main(int argc, char* argv[])
{
    /*Inizializzo i segnali*/
    sigemptyset(&zeromask);
    sigemptyset(&set);
    sigaddset(&set,SIGUSR1);
    sigaddset(&set,SIGUSR2);
    sigprocmask(SIG_BLOCK,&set,NULL);
    
    sigemptyset(&act1.sa_mask);
    act1.sa_handler = storeIn;
    act1.sa_flags = SA_RESTART;
    if((sigaction(SIGUSR1,&act1,NULL)) < 0){
        perror("Errore nella sigaction 1");
        exit(-1);
    }
    sigemptyset(&act2.sa_mask);
    act2.sa_handler = clearAll;
    act2.sa_flags = SA_RESTART;
    if((sigaction(SIGUSR2,&act2,NULL)) < 0){
        perror("Errore nella sigaction 2");
        exit(-1);
    }
    
    /*Inizializzo la pipe di comunicazione tra processo Server e processo Store*/
    if((pipe(piped)) < 0){
        perror("Errore nella creazione della pipe");
        exit(-1);
    }
    
    /* Crea la  socket STREAM */
    sock = socket(AF_INET,SOCK_STREAM,0);
    if(sock<0){
        perror("opening stream socket");
        exit(-1);
    }
    portno = PORT;   /*Perchè le porte con numero < 1024 sono riservate ai servizi di rete di sistema*/
    /* Utilizzo della wildcard per accettare le connessioni ricevute da ogni interfaccia di rete del sistema */
    server.sin_family = AF_INET;
    server.sin_addr.s_addr= INADDR_ANY;
    server.sin_port = htons(portno);
    on = 1;
    rval = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    if (bind(sock,(struct sockaddr *)&server,sizeof(server))<0){
        perror("binding stream socket");
        exit(1);
    }
    length= sizeof(server);
    if(getsockname(sock,(struct sockaddr *)&server,&length)<0){
        perror("getting socket name");
        exit(1);
    }
    printf("Socket port #%d\n",ntohs(server.sin_port));
    
    /*Creazione del processo Pstore*/
    if((pidstore = fork()) < 0){
        perror("Errore nella fork()");
        exit(-1);
    }
    else{  
        if(pidstore == 0){  /*Codice del proceso Pstore*/
            close(piped[1]);
            for(i=0;i<10;i++)
                memoria[i]=0;
            sigprocmask(SIG_SETMASK,&zeromask,NULL); /*Abilito la ricezione dei segnali SIGUSR1 e SIGUSR2 per il processo Pstore*/
            printf("Salve, solo il processo di salvataggio, il mio PID e': %d\n",getpid());
            bodyStore();
            exit(0);
        }
    }
    /*Codice del processo Pserver*/
    listen(sock,2);
    do {
        /* Attesa di una connessione */	 
        msgsock = accept(sock,(struct sockaddr *)&client,(socklen_t *)&length);
        if(msgsock ==-1)
            perror("accept");
        else{ 
            if((pid = fork())== 0){
                close(sock); close(piped[0]);
                /* Esecuzione del servizio */
                if((rval = read(msgsock,message,sizeof(message))) < 0){
                    perror("Errore nella read del server");
                    exit(-1);
                }
                /* Passaggio delle informazioni al processo Store */
                write(piped[1],message,strlen(message));
                exit(0);     
            }
            else{
                if(pid == -1){ /* Errore nella fork */
                    perror("Error on fork: ");
                    exit(-1);
                }
                else{
                    /* OK, il padre torna in accept */
                    close(msgsock);
                }
            }  
        }
    }
    while(1);
    close(sock);
    exit(0);
}
