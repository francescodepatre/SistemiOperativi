#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <sys/socket.h>
#include<fcntl.h>
#include<netdb.h>
#include<netinet/in.h>
#include<string.h>
void handler(int signo){
	printf("Segnale ricevuto\n");
	//body handler
	
}
int main(int argc, char **argv){
    
    //preparazione segnale SIGUSR1
    sigset_t set,zeromask;
    struct sigaction action;
    sigemptyset(&zeromask);   //prepara una maschera di segnali vuota
        
    sigemptyset(&action.sa_mask); //azzeramento dei flag in sa_mask presente all'interno della struttura action
    action.sa_handler = handler; //assegno la funzione da chiamare durante la ricezione del segnale
    action.sa_flags =0;
    sigemptyset(&set); 
    sigaddset(&set, SIGUSR1); 
    sigprocmask(SIG_BLOCK, &set , NULL); //modifichiamo la signal mask di sigset set

    if(sigaction(SIGUSR1, &action, NULL)==-1){
        perror("Sigaction error");
    }
    
    //variabili socket
	int sockfd,portno=1111,msgsock,lenght;
    int portno2=5555,sockfd2,msgsock2,lenght2; //SIGUSR1
    int portno3=9999,sockfd3,msgsock3,lenght3; //SIGUSR2
	char buffer[256];
	int op1;
    int M=1;
    int q;
    q=atoi(argv[1]);
    
    
    
	struct sockaddr_in server1,server2,client;
	
	//creazione prima socket
	sockfd = socket(AF_INET,SOCK_STREAM,0);
	if(sockfd<0){
		perror("Sock error");
	}
	
	//definizione struttura socket
	server1.sin_family = AF_INET;
	server1.sin_addr.s_addr = INADDR_ANY;
	server1.sin_port = htons(portno);
	lenght = sizeof(server1);
	
	//assegno nome alla socket
	if(bind(sockfd,(struct sockaddr*)&server1,sizeof(server1))<0){
		perror("Error");
	}
	
	if(getsockname(sockfd,(struct sockaddr*)&server1,(socklen_t*)&lenght)<0){
		perror("Error");
	}
	
	//ascolto
	listen(sockfd,5);
    
    int pid1;
	char recive[256];
	
	
	do{
        
            msgsock = accept(sockfd,(struct sockaddr*)&client,(socklen_t*)&lenght);
            
            if((pid1=fork())==0){
                
                close(sockfd);
                read(msgsock,recive,sizeof(recive));
                
                sscanf(recive,"%d",&op1);
                if((op1%100)==0){
                    M=(0.01*op1);
                }
                printf("operazione:(M*op1)+q = (%d * %d)+%d\n",M,op1,q);
                int risposta = (M*op1)+q;
                
                sprintf(buffer,"%d",risposta);
                printf("Risposta:  = %d\n",risposta);
                write(msgsock,buffer,strlen(buffer));
                close(msgsock);
                exit(0);
            
    }
    else{close(msgsock);}
}while(1);
}
