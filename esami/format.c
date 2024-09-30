#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <time.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <signal.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define TRUE (1==1)
#define FALSE (1==0)

#define Buff_DIM 256


int piped[2];
//int N = 10;
//int piped[N][2]

int condition = FALSE;



void handler(int signo)
{
	if (signo == SIGUSR1)
	{
	}
}

int main(int argc, char *argv[])
{
	int port_n, lenght;
	int ret, retcode, socktname;
	int sockt, msgsockt;
	unsigned int client_lenght;
	struct sockaddr_in server, client;
	
	int server_pid, pid;
	
	if (argc != 2)
	{
		perror("Use: %s <portnumber>\n", argv[0]);
		exit(1);
	}
	
	if ( pipe(piped) < 0)
	{
		perror("creating pipe");
		exit(2);
	}
	
	server_pid = getpid();
	printf("Server pid: %dn", server_pid);
	
	struct sigaction sig;
	sig.sig_handler = handler;
	sigemptyset(&sig.sa_mask);
	sig.sa_flags = SA_RESTART;
	sigaction(SIGUSR1, &sig, NULL);
	
	sockt = socket(AF_INET, SOCK_STREAM, 0);
	if ( sockt < 0)
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
	if (ret < 0)
	{
		perror("setsockopt reuseaddr");
		exit(4);
	}
	
	retcode = bind(sockt, (struct sockaddr *)&server, lenght);
	if (retcode < 0)
	{
		perror("bind");
		exit(5);
	}
	
	socktname = getsockname(sockt, (struct sockaddr *)&server, &lenght);
	if (socktname < 0)
	{
		perror("getting socket name");
		exit(6);
	}
	
	listen(sockt, 4);
	
	do
	{
		client_lenght = sizeof(client);
		msgsockt = accept(sockt, (struct sockaddr *)&client, &client_lenght);
		if (msgsockt < 0)
		{
			perror("accept");
			exit(7);
		}
		else
		{
			printf("Connected to %s on port #%d\n", inet_ntoa(client.sin_addr), port_n);
			pid = fork();
			
			if (pid < 0)
			{
				perror("fork");
				exit(8);
			}
			else if (pid == 0)
			{
				close(sockt);
				/* body */
				close(msgsockt);
				exit(0);
			}
			else
			{
				close(msgsockt);
			}
		}
	} while (1);
	
	close(sockt);
	exit(0);
}
