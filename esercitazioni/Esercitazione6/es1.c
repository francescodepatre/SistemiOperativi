#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/wait.h>

#define N 256

int main(int argc, char *argv[])
{
	int sock,sockfd,length,on,ret,o,portno;
	char  buffer[N];
	struct sockaddr_in server,client;
	int pid, status;
	
	if(argc != 2)
    {
		fprintf(stderr,"Uso: %s <numero_porta>\n",argv[0]);
		exit(1);
	}
	
	/*creazione e controllo socket*/
	sock = socket(AF_INET,SOCK_STREAM,0);
	
	if(sock < 0)
    {
		perror("Socket error");
		exit(2);
	}
	
	on = 1;
	ret = setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));
	
	if(ret < 0)
    {
		perror("Setsockopt error");
		exit(3);
	}
	
	portno = atoi(argv[1]);
	
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(portno);
	
	/*controllo bind*/
	if(bind(sock,(struct sockaddr *)&server,sizeof(server)) < 0)
    {
		perror("Bind error");
		exit(4);
	}
	
	length = sizeof(client);
	
	listen(sock,4);
	printf("SERVER: in ascolto sulla porta # %d\n",ntohs(server.sin_port));
	
	/*chiamata e controllo accept*/
	sockfd = accept(sock,(struct sockaddr *)&client,(socklen_t *)&length);
	
	if(sockfd < 0)
    {
		perror("Accept error");
		exit(5);
	}
	if((pid = fork()) < 0)
    {
		perror("Fork error");
		exit(6);
	}
		
	else if(pid == 0)
    {
		/*lettura e controllo*/
		o = read(sockfd,buffer,N);
		
		if(o < 0){
			perror("Read error");
			exit(6);
			}
		
		printf("SERVER: ecco il messaggio ricevuto dal client -> %s\n",buffer);
		
		/*scrittura e controllo*/
		o = write(sockfd,"MESSAGGIO RICEVUTO FINE COMUNICAZIONE\n",38);
		
		if(o < 0){
			perror("Write error");
			exit(7);
			}

        close(sock);
		close(sockfd);
	}
	else
    {
		wait(&status);
		printf("SERVER: chiusura socket");
		close(sock);
		close(sockfd);
		printf("SERVER: termino\n");
		exit(0);
	}
}