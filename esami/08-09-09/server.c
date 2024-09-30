#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/timeb.h>
#include <sys/types.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFER_DIM 256

#define TRUE  (1==1)
#define FALSE (1==0)

int log_is_active = FALSE;
//int log_is_active = TRUE;

void sigusr_handler(int signo)
{
    if (signo == SIGUSR1)
    {
        log_is_active = TRUE;
        printf("Log is now active\n");
    }
    else if (signo == SIGUSR2)
    {
        log_is_active = FALSE;
        printf("Log is now inactive\n");
    }
}



int main(int argc, char *argv[])
{
    int port_n, lenght, pid, server_pid;
    int a, b, sum, prod;
    int sockt, msgsockt;
    int retcode, socktname;
    unsigned int client_lenght;
    struct sockaddr_in server, client;
    
    char line_out[BUFFER_DIM], line_in[BUFFER_DIM], answer[BUFFER_DIM];


    if (argc != 2)
    {
        printf("Use: %s <port_number>\n", argv[0]);
        exit(1);
    }

	server_pid = getpid();
	printf("Server pid: %d\n", server_pid);     
       
    /*gestione affidabile segnali*/
	struct sigaction sig;
    sig.sa_handler = sigusr_handler;
    sigemptyset(&sig.sa_mask);
    sig.sa_flags = SA_RESTART;
    sigaction(SIGUSR1, &sig, NULL);
    sigaction(SIGUSR2, &sig, NULL);
    

    /*gestione socket*/
    port_n = atoi(argv[1]);

    sockt = socket(AF_INET, SOCK_STREAM, 0);
    if (sockt < 0)
    {
        perror("creating socket");
        exit(2);
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port_n);

    lenght = sizeof(server);


    int on=1;
    if (setsockopt(sockt,SOL_SOCKET, SO_REUSEADDR,&on,sizeof(on))<0)
    {
        perror("setsockopt reuseaddr sockt");
    }

    retcode = bind(sockt, (struct sockaddr *)&server, lenght);
    if (retcode < 0)
    {
        perror("binding socket");
        exit(3);
    }

    socktname = getsockname(sockt, (struct sockaddr *)&server, &lenght);
    if (socktname < 0)
    {
        perror("getting socket name");
        exit(4);
    }

    listen(sockt, 4);

    printf("Server listening on port # %d\n", port_n);
    
    do
    {
        //client_lenght = sizeof(client);
        msgsockt = accept(sockt, (struct sockaddr *)&client, (socklen_t *)&lenght);
        if(msgsockt < 0)
        {
        	perror("accept");
        	exit(5);
        }
        else
        {
		    printf("Connessione a %s sulla porta # %d\n", inet_ntoa(client.sin_addr), port_n);

		    pid = fork();
		    if (pid == -1)
		    {
		        perror("Creating child process");
		        exit(6);
		    }
		    else if (pid == 0)
		    {
		    	close(sockt);
		       
		        sprintf(line_out, "SERVER: insert 2 integer numbers:\n");
		        write(msgsockt, line_out, strlen(line_out)+1);
		        
		        read(msgsockt, line_in, sizeof(line_in));
		        sscanf(line_in, "%d %d", &a, &b);
		        
		        sum = a + b;
		        prod = a * b;
		        
		        if(log_is_active)
		        {
		        	printf("Received: %d and %d\n", a, b);
		        	printf("Sum: %d\nProduct: %d\n", sum, prod);
		        }
		        
		        sprintf(answer, "SERVER: sum: %d\nSERVER: product: %d\n", sum, prod);
		        write(msgsockt, answer, strlen(answer)+1);
		        printf("Answer sent\nClosing connection with %s\n", inet_ntoa(client.sin_addr));
		        
		        close(msgsockt);
		        exit(0);
		        
		    }
		    else
		    {
		    	close(msgsockt);
		    }
		}
    } while (1);
}
