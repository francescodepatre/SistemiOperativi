/*
Author Luca Coloretti 321998
*/
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <signal.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define BUFF_DIM 256

#define TRUE (1==1)
#define FALSE (1==0)


int port_n, lenght, P, Q;
int server_pid, pid;
unsigned int client_lenght;
char line_out[BUFF_DIM], line_in[BUFF_DIM], answer[BUFF_DIM];

int global_max;
int piped[2];

int mode1 = FALSE;

void handler(int signo)
{
	if(signo == SIGUSR1)
	{
		if(mode1)
		{
			mode1 = FALSE;
			printf("Mode 1 inactive\n");
		}
		else
		{
			mode1 = TRUE;
			printf("Mode 1 is active\n");
			read(piped[0], &global_max, sizeof(int));
			printf("Current max global value: %d\n", global_max);
			write(piped[1], &global_max, sizeof(int));
		}
	}
	else if(signo == SIGUSR2)
	{
		read(piped[0], &global_max, sizeof(int));
		printf("Max global value: %d\n", global_max);
		global_max = 0;
		printf("Max global value now set to 0.\n");
		write(piped[1], &global_max, sizeof(int));
	}
}

int max_beetween(int a, int b)
{
	if (a >= b) { return a;}
	else {return b;}
}

int main (int argc, char *argv[])
{
	int ret, retcode, socktname, rval;
	int sockt, msgsockt;
	struct sockaddr_in server, client;
	
	if(argc != 2)
	{
		printf("Use: %s <port_number>", argv[0]);
		exit(1);
	}
	
	if(pipe(piped) < 0)
	{
		perror("Creating pipe");
		exit(2);
	}
	global_max = 0;
	write(piped[1], &global_max, sizeof(int));
	
	server_pid = getpid();
	printf("Server pid: %d\n", server_pid);
	
	/*gestione affidabile dei segnali*/
	struct sigaction sig;
	sig.sa_handler = handler;
	sigemptyset( &sig.sa_mask);
	sig.sa_flags = SA_RESTART;
	sigaction(SIGUSR1, &sig, NULL);
	sigaction(SIGUSR2, &sig, NULL);
	
	sockt = socket(AF_INET, SOCK_STREAM, 0);
	if(sockt < 0)
	{
		perror("creating socket");
		exit(3);
	}
	
	port_n = atoi(argv[1]);
	
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(port_n);
	
	lenght = sizeof(server);
	
	int on = 1;
	ret = setsockopt(sockt, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
	if(ret < 0)
	{
		perror("setsockopt reuseaddr");
		exit(4);
	}
	
	retcode = bind(sockt, (struct sockaddr *)&server, lenght);
	if(retcode < 0)
	{
		perror("bind");
		exit(4);
	}
	
	socktname = getsockname(sockt, (struct sockaddr *)&server, &lenght);
	if(retcode < 0)
	{
		perror("bind");
		exit(4);
	}
	
	printf("Server listening on port # %d\n", port_n);
	
	listen(sockt, 2);
	
	do
	{
		client_lenght = sizeof(client);
		msgsockt = accept(sockt, (struct sockaddr *)&client, &client_lenght);
		if(msgsockt < 0)
		{
			perror("accept");
			exit(5);
		}
		else
		{
			printf("Server connected to %s on port #%d\n", inet_ntoa(client.sin_addr), port_n);
			pid = fork();
			if(pid < 0)
			{
				perror("fork");
				exit(6);
			}
			else if (pid == 0)
			{
				close(sockt);
				sprintf(line_out, "SERVER: Insert two positive integer numbers (n >= 0):\n");
				write(msgsockt, &line_out, strlen(line_out)+1);
				
				rval = read(msgsockt, &line_in, sizeof(line_in));
				if(rval < 0)
				{
					perror("reading client request");
					exit(7);
				}
				else
				{
					int scan = sscanf(line_in, "%d %d", &P, &Q);
					if (scan < 0 || P <= 0 || Q <= 0)
					{
						sprintf(answer, "SERVER: Input error\n");
						write(msgsockt, &answer, strlen(answer)+1);
						close(msgsockt);
						exit(8);
					}
					else
					{
						int max = max_beetween(P, Q);
						sprintf(answer, "SERVER: Max value beetween %d and %d is: %d\n", P, Q, max);
						write(msgsockt, &answer, strlen(answer)+1);
						if(P == Q)
						{
							sprintf(answer, "SERVER: The two numbers are equal\n");
							write(msgsockt, &answer, strlen(answer)+1);
						}
						
						if(mode1)
						{
							read(piped[0], &global_max, sizeof(int));
							int curr_max = max_beetween(max, global_max);
							if(global_max < max){printf("New global max value is: %d\n", curr_max);}
							sprintf(answer, "SERVER: Max value beetween %d, %d and global max value %d is: %d\n", P, Q, global_max, curr_max);
							write(msgsockt, &answer, strlen(answer)+1);
							if(global_max == max)
							{
								sprintf(answer, "SERVER: The two numbers are equal\n");
								write(msgsockt, &answer, strlen(answer)+1);
							}
							write(piped[1], &curr_max, sizeof(int));
						}
						close(msgsockt);
						exit(0);
					}
				}
			}
			else
			{
				close(msgsockt);
			}
			
		}

	} while(1);

	close(sockt);
}




