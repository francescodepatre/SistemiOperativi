#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#define DEFAULTPORT 1234

#define MINSERVERS 5
#define MAXSERVERS 10


int got_sigusr1 =  0;

void handler(int signo) {
    
    got_sigusr1 = 1;
       
}

main(int argc, char *argv[])
{
  int sock, length;
  struct sockaddr_in server, client;
  int i, msgsock, rval,pid;
  struct hostent *hp, *gesthostbyname();
  int myport=1234;
  int numero, status;
  struct sigaction sig;
  
  
  if(argc==2)
    numero=atoi(argv[1]);
  else {
      fprintf(stderr,"Uso: %s numero\n",argv[0]);
      exit(1);
  }
    
    
  sig.sa_handler = handler; //installo il gestore del segnale SIGUSR1
  sigemptyset(&sig.sa_mask);
  sig.sa_flags=SA_RESTART;

  sigaction(SIGUSR1,&sig,NULL);
  
    

  sock=socket(AF_INET, SOCK_STREAM, 0);
  if (sock<0)
    {
      perror("errore creazione stream socket");
      exit(1);
    }
  server.sin_family= AF_INET;
  server.sin_addr.s_addr=INADDR_ANY;
  server.sin_port=htons(myport);
  
  int on=1;
  if (setsockopt(sock,SOL_SOCKET, SO_REUSEADDR,&on,sizeof(on))<0) {
            perror("setsockopt reuseaddr");
     }

  

  if(bind(sock, (struct sockaddr *) &server, sizeof server)<0)
  {
    perror("bind su stream socket");
    exit(1);
  }
  length=sizeof server;

  if(getsockname(sock,(struct sockaddr *) &server, &length)<0)
  { perror("Getsockname"); exit(-1);}

     printf ("Porta della socket del server = #%d\n", ntohs(server.sin_port));

     if (listen(sock,2)<0) {
        perror("listen"); 
        exit(2);
        }

     while(1) {
        
     if((msgsock=accept(sock, (struct sockaddr *)&client, &length))<0) {
           perror("Accept");
           exit(3);
           }
           
     if (got_sigusr1)
        printf("Server %d : accepting request from client %s:%d\n",getpid(),inet_ntoa(client.sin_addr ),client.sin_port);
         
           
           
/*******************************************************************************************************************
Come fare in modo che le modifiche al numero effettuate
in un processo figlio del server siano disponibili a tutti i figli creati successivamente ?
Possibili soluzioni:

1) processo gestore lato server che riceve il nuovo numero da un figlio del server (che lo ha ottenuto da un client)
e lo fornisce ai figli del server che lo richiedono

2) si può utilizzare un pipe in cui un figlio del server scrive il nuovo valore
e da cui il server il padre lo recupera senza bloccarsi (select seguita da read)

3) si può anche utilizzare una forma implicita di comunicazione tra un figlio del server
e il padre attraverso exit(val) del figlo (val deve essere un valore inferiore a 128/char o a 255/unsigned char)
che consente al padre di recuperare il valore val
mediante una wait (bloccante) o meglio la sua forma non bloccante waitpid(-1, &status, WNOHANG); 
QUESTA E' LA SOLUZIONE UTILIZZATA IN QUESTO SORGENTE
********************************************************************************************************************/   
     
      pid = waitpid(-1, &status, WNOHANG);  // check if child ended without waiting for it
      
      if (pid>0 && WEXITSTATUS(status)>0 &&WEXITSTATUS(status) < 128 ) {
          numero = WEXITSTATUS(status);   // get the new value from exit status of the child
          printf("Nuovo numero=%d\n",numero);  
      }
   
        if (fork() == 0) {
                      
            
                close(sock);
                numero= service(msgsock,numero);
                close(msgsock);
                exit(numero);
                 
                }
        close(msgsock);
    }

    
}
    
    
int sendPromptReceiveAnswer(int csock,char *prompt,char answer[64],char *stringToMatch)   {
char buf[128];

    
    if(write(csock,prompt,strlen(prompt)+1) <0 ) {
        perror("write prompt");
        exit(5);
        }
        
        if(read(csock, buf, sizeof buf)<0) {
        perror("read");
        exit(6);
        } 

        strcpy(answer,buf);
        
        return strncmp(buf,stringToMatch,strlen(stringToMatch));
        

}    
    
int readCmd(int csock,char *string) {
    char *cmd="Command ? ";
    char buf[128];
    int num;
    
          if (!sendPromptReceiveAnswer(csock,cmd,buf,string)) {
            
            if (!strncmp(buf,"WRITE",5)) {
                sscanf(buf, "WRITE %d",&num);
                return num;    
            }
            else return 0;  
            
        }
       else return -1; 
}

    

   

int service(int csock, int numero) {
    int rval,n;
    char buf[128];
    char *strLogin="Login: ";
    char *valueLogin="admin";
    char *strPassword="Password: ";
    char *valuePassword="so-1718";
    char *strWrongCommand="Wrong command\n";
   
        
    if (!sendPromptReceiveAnswer(csock,strLogin,buf,valueLogin)) {    
       
         if (!sendPromptReceiveAnswer(csock,strPassword,buf,valuePassword)) {
             
               numero = readCmd(csock,"WRITE");    
               if (numero > 0)
                    exit(numero);  // Implicitly send the new value for the number to father process          
               else
                   if(write(csock,strWrongCommand,strlen(strWrongCommand)+1) <0 ) {
                    perror("write wrong command");
                    exit(-9);
                    }
               
            }
         /* ALLOW FOR READ */
        }  
        
        /* ONLY READ */
        
        if (readCmd(csock,"READ") == 0) {   
            sprintf(buf,"%d\n",numero);     
            if(write(csock,buf,strlen(buf)+1) <0 ) {
                perror("write Password");
                exit(-10);
                }
        
            
            }
         else {
             
             if(write(csock,strWrongCommand,strlen(strWrongCommand)+1) <0 ) {
                perror("write wrong command");
                exit(-11);
                }
             
         }
         exit(0);

    
    
}
