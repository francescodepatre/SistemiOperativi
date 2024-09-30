#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
//#include <sys/stat.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <time.h>
//#include <errno.h>

#define BUFF_SIZE 256


#define TRUE (1==1)
#define FALSE (1==0)


//int proc_accepted = 1;

int service_is_active = TRUE;
int resource_is_occupied = FALSE;

void handler(int signo)
{
	if (signo == SIGUSR1)
	{
		if (service_is_active)
		{
			service_is_active = FALSE;
			printf("Service is now inactive.\n");
		}
		else
		{
			service_is_active = TRUE;
			printf("Service is now active.\n");
		}
	}
	else if (signo == SIGUSR2)
	{
		if (resource_is_occupied)
		{
			resource_is_occupied = FALSE;
			printf("Resource R is now free.\n");
			//proc_accepted += 1;
		}
		else
		{
			resource_is_occupied = TRUE;
			printf("Resource R is now occupied.\n");
		}
	}
}

int main (int argc, char *argv[])
{
	int port_n, lenght, server_pid, pid, id, idb;
	int sockt, msgsockt;
	unsigned int client_lenght;
	int ret, retcode, socktname;
	struct sockaddr_in server, client;
	char line_out[BUFF_SIZE], line_in[BUFF_SIZE], answer[BUFF_SIZE], line_in2[BUFF_SIZE];
	
	char in;
	char p = 'P';
	
	if (argc != 2)
	{
		printf("Use: %s <port_number>\n", argv[0]);
		exit(1);
		
	}
	
	int acc[2];
	if(pipe(acc)<0)
	{
		perror("creating pipe");
		exit(11);
	}
	int m = 1;
	write(acc[1], &m, sizeof(int));
	
	/*gestione affidabile dei segnali*/
	struct sigaction sig;
	sig.sa_handler = handler;
	sigemptyset(&sig.sa_mask);
	sig.sa_flags = SA_RESTART;
	
	sigaction(SIGUSR1, &sig, NULL);
	sigaction(SIGUSR2, &sig, NULL);
	
	
	sockt = socket(AF_INET, SOCK_STREAM, 0);
	if (sockt < 0)
	{
		perror("naming socket");
		exit(3);
	}
	
	server_pid = getpid();
	printf("Server pid: %d\n", server_pid);
	
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
	
	printf("SERVER LISTENING ON PORT # %d...\n", port_n);
	
	int curr_proc = 0;
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
			printf("SERVER CONNECTED TO: %s ON PORT # %d\n", inet_ntoa(client.sin_addr), port_n);
			
			curr_proc += 1;
			
			srand(time(NULL));
			id = rand()%1000001 +1;
			
			pid = fork();
			if (pid < 0)
			{
				perror("creating child process");
				exit(8);
			}
			else if (pid == 0)
			{
				close(sockt);
				if (service_is_active)
				{
					sprintf(line_out, "Need resource? (press 'P' to confirm)\n");
					write(msgsockt, &line_out, strlen(line_out)+1);
					
					read(msgsockt, &line_in, sizeof(line_in));
					sscanf(line_in, "%c", &in);
					
					if (in == p)
					{
						if (resource_is_occupied)
							{
								sprintf(answer, "Sorry, resource is currently occupied.\n");
								write(msgsockt, &answer, strlen(answer)+1);
							}
						do
						{	
							int proc_accepted;
							read(acc[0], &proc_accepted, sizeof(int));
							write(acc[1], &proc_accepted, sizeof(int));
							if (resource_is_occupied == FALSE && curr_proc == proc_accepted)  { break;}
						} while(1);
						
						kill(server_pid, SIGUSR2);
						sprintf(answer, "Resource assigned: ID = %d \n", id);
						write(msgsockt, &answer, strlen(line_out)+1);
						
						do
						{
							read(msgsockt, &line_in, sizeof(line_in));
							sscanf(line_in, "P %d", &idb);
							if (idb == id)
							{
								int proc_accept;
								read(acc[0], &proc_accept, sizeof(int));
								kill(server_pid, SIGUSR2);
								proc_accept += 1;
								write(acc[1], &proc_accept, sizeof(int));
								close(msgsockt);
								exit(0);
							}
						} while (1);
					}
					else
					{
						close(msgsockt);
						exit(0);
					}
				}
				else
				{
					sprintf(line_out, "Sorry service is not on line\n");
					write(msgsockt, &line_out, strlen(line_out)+1);
					close(msgsockt);
					exit(0);
				}
			}
			else
			{
				//wait(0);
				int proc_accepted;
				read(acc[0], &proc_accepted, sizeof(int));
				write(acc[1], &proc_accepted, sizeof(int));
				close(msgsockt);
			}
		}
	
	} while (1);
	
	close(sockt);
	printf("Closing server\n");
	exit(0);
}
