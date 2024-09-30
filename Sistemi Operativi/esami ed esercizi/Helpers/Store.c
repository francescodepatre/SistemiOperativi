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


int vett[10];

void handler(int signo){
	printf("ciao");
	for(int i =0;i<10;i++){
		printf("Vett[%d]= %d\n", i, vett[i]);
	}
}

int main(int argc, char **argv){

		//preparazione segnale SIGUSR1
sigset_t set,zeromask;
struct sigaction sig;
sigemptyset(&zeromask); 
	

sig.sa_handler = handler;
sigemptyset(&sig.sa_mask);
sigaddset(&sig.sa_mask, SIGUSR1); 
sig.sa_flags =SA_RESTART;


if(sigaction(SIGUSR1, &sig, NULL)==-1){
perror("Sigaction error");}
	int pip[2];
	pipe(pip);

	//variabili generali e socket
	int sock,msgsock,lung;
	char buffer[256];
	char comando[256];
	int indirizzo, valore;
	struct sockaddr_in server,client;
	
	int portno = atoi(argv[1]);
	int pid;
	//creazione socket
	sock = socket(AF_INET, SOCK_STREAM,0);
	if(sock<0){
		perror("Sock error");
		exit(0);
	}
	
	//definizione struttura socket
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(portno);
	
	lung = sizeof(server);
	
	//binding e naming
	if(bind(sock, (struct sockaddr *)&server, sizeof(server))<0){
		perror("Bind error");
		exit(0);
	}
	
	if(getsockname(sock, (struct sockaddr *)&server, (socklen_t*)&lung)<0){
		perror("Getsockname error");
		exit(0);
	}
	
	
	//ascolto
	listen(sock,5);
	
	printf("Socket port %d, il mio pid %d\n", ntohs(server.sin_port), getpid());
	
	do{
		msgsock = accept(sock,(struct sockaddr *)&client, (socklen_t *)&lung);
		if(msgsock<0){
			perror("Accept error");
			exit(0);
		}
		
		
		if((pid = fork())<0){
			perror("Fork error");
			exit(0);
		}
		
		else if(pid == 0){
			close(sock);
			printf("Il mio pid %d\n", getpid());
			//write(msgsock,"Ciao sono il server, cosa ti serve(r)?\n",50);
			read(msgsock,buffer,sizeof(buffer));
			sscanf(buffer, "%s %d %d", comando, &indirizzo,&valore);
			printf("Ho ricevuto %s, %d, %d, questo è pid %d\n", comando, indirizzo, valore,getpid());
			if(strcmp(comando,"store")==0){
			printf("SO");
			vett[indirizzo] = valore;
			} 
			sig.sa_flags = SA_RESTART;
			close(msgsock);
			exit(0);
		}
		else{
			close(msgsock);
		}
	
	
	
	}while(1);
	
	
	
	exit(0);
}
