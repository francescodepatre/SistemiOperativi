#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define BUFFER_DIM 256
#define TRUE (1==1)
#define FALSE (1==0)

int MODE1 = FALSE;
int SUSPEND = FALSE;

void handler(int signo)
{
	if(signo == SIGUSR1)
	{
		MODE1 == TRUE;
	}
	else if (signo == SIGUSR2)
	{
		SUSPEND == TRUE;
	}
}


int main(int argc, char *argv[])
{
	int port_n, lenght, end;
	int server_pid, pid, client_lenght;
	int ret, retcode, sockt_name;
	int nrand, nmax, n;
	int sockt, msgsockt;
	struct sockaddr_in server, client;
	char line_out[BUFFER_DIM], line_in[BUFFER_DIM], answer[BUFFER_DIM];
	
	int piped[2];
	if (pipe(piped)<0)
	{
		perror("Creating pipe");
		exit(1);
	}
	
	/*Gestione affidabile dei segnali
	struct sigaction sig;
	sig.sa_handler = handler;
	sigemptyset(&sig.sa_mask);
	sig.sa_flags = SA_RESTART;
	
	sigaction(SIGUSR1, &sig, NULL);
	sigaction(SIGUSR2, &sig, NULL);
	*/
	
	if (argc != 3)
	{
		printf("Use: %s <port_number> <max_number>", argv[0]);
		exit(2);
	}
	

	/*ottengo il pid per poter inviare SIGUSR1/2*/
	server_pid = getpid();
	nmax = atoi(argv[2]);
	srand(time(NULL));
	nrand = rand()%nmax;
	write(piped[1], &nrand, sizeof(int));
	printf("SERVER PID: # %d\n", server_pid);
	printf("N max: %d\n", nmax);
	printf("Win value: %d\n", nrand);
	
	/*creazione socket*/
	sockt = socket(AF_INET, SOCK_STREAM, 0);
	if (sockt < 0)
	{
		perror("Creating socket");
		exit(3);
	}

	port_n = atoi(argv[1]);	
	
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(port_n);
	
	lenght = sizeof(server);
	
	int on = 1;
	ret = setsockopt(sockt, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
	if (ret < 0)
	{
		perror("setsockopt reuseaddr sockt");
		exit(4);
	}
	
	retcode = bind(sockt, (struct sockaddr *)&server, lenght);
	if(retcode < 0)
	{
		perror("bind");
		exit(5);
	}
	
	sockt_name = getsockname(sockt, (struct sockaddr *)&server, &lenght);
	if(sockt_name < 0)
	{
		perror("getting socket name");
		exit(6);
	}
	
	listen(sockt, 2);
	
	printf("Server listening on port # %d\n", port_n);
	printf("Waiting for connection...\n");
	
	end = 0;
	
	do
	{
		client_lenght = sizeof(client);
		msgsockt = accept(sockt, (struct sockaddr *)&client, &client_lenght);
		if(msgsockt < 0)
		{
			perror("accept");
			exit(7);
		}
		else
		{
			printf("Connected to %s on port # %d\n", inet_ntoa(client.sin_addr), port_n);
			pid = fork();
			if(pid < 0)
			{
				perror("fork error");
				exit(8);
			}
			else if (pid == 0)
			{
				close(sockt);
				
				sprintf(line_out, "Insert an integer number\n");
				write(msgsockt, &line_out, strlen(line_out)+1);
				
				read(msgsockt, line_in, sizeof(line_in));
				sscanf(line_in, "%d", &n);
				
				read(piped[0], &nrand, sizeof(int));
				if (n == nrand)
				{
					sprintf(answer, "You won!\n");
					write(msgsockt, &answer, strlen(answer)+1);
					srand(time(NULL));
					nrand = rand()%nmax;
					printf("New win value: %d\n", nrand);
					
				}
				else
				{
					sprintf(answer, "Sorry, you lost :(\n");
					write(msgsockt, &answer, strlen(answer)+1);
				}
				write(piped[1], &nrand, sizeof(int));
				
				close(msgsockt);
				exit(0);
			}
			else
			{
				close(msgsockt);
			}
			
		
		
		
		
		}
		
		
		
	} while(end != 1);

	exit(0);

}
