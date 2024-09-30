#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>

#define BUFF_DIM 256

#define TRUE (1==1)
#define FALSE (1==0)

int service_is_active = TRUE;

typedef struct _book
{
	int book_id;
	char title[64];
	char author[64];
	int available;
	int code;
} book;

void handler(int signo)
{
	if (signo == SIGUSR1)
	{
		if (service_is_active)
		{
			service_is_active = FALSE;
			printf("Service is now suspended\n");
		}
		else
		{
			service_is_active = TRUE;
			printf("Service is now active\n");
		}
	}
}



int main(int argc, char *argv[])
{
	int port_n, lenght, server_pid, pid, book_n;
	int ret, retcode, socktname;
	int sockt, msgsockt;
	unsigned int client_lenght;
	struct sockaddr_in server, client;
	char line_out[BUFF_DIM], line_in[BUFF_DIM], answer[BUFF_DIM];
	
	
	if (argc != 3)
	{
		printf("Use: %s <port_number> <N>\n", argv[0]);
		printf("with 3 < N < 11\n");
		exit(1);
	}
	
	book_n = atoi(argv[2]);
	if(book_n < 4 || book_n > 10)
	{
		printf("Book number must be 3 < N < 11\n");
		exit(11);
	}

	int piped[book_n][2];
	for(int i = 0; i < book_n; i++)
	{
		if(pipe(piped[i]) < 0)
		{
			perror("Creating pipe");
			exit(12);
		}
	}
	
	book bookd;
	char name[64], auth[64];
	for (int i = 0; i < book_n; i++)
	{
		int n = i + 1;
		sprintf(name, "Title%d", n);
		sprintf(auth, "Author%d", n);
		bookd.book_id = n;
		strcpy(bookd.title, name);
		strcpy(bookd.author, auth);
		bookd.available = TRUE;
		write(piped[i][1], &bookd, sizeof(bookd));
	}
	
	
	server_pid = getpid();
	printf("SERVER PID: # %d\n", server_pid);
	
	struct sigaction sig;
	sig.sa_handler = handler;
	sigemptyset(&sig.sa_mask);
	sig.sa_flags = SA_RESTART;
	
	sigaction(SIGUSR1, &sig, NULL);
	
	sockt = socket(AF_INET, SOCK_STREAM, 0);
	if (sockt < 0)
	{
		perror("creating socket");
		exit(2);
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
		exit(3);
	}
	
	retcode = bind(sockt, (struct sockaddr *)&server, lenght);
	if(retcode < 0)
	{
		perror("bind");
		exit(4);
	}
	
	socktname = getsockname(sockt, (struct sockaddr *)&server, &lenght);
	if(socktname < 0)
	{
		perror("getting socket name");
		exit(5);
	}
	
	listen(sockt, 2);
	
	printf("Server listening on port # %d\n", port_n);
	
	do
	{
		client_lenght = sizeof(server);
		msgsockt = accept(sockt, (struct sockaddr *)&client, &client_lenght);
		
		if (msgsockt < 0)
		{
			perror("accept");
			exit(6);
		}
		else
		{
			printf("Server connected to %s on port #%d\n", inet_ntoa(client.sin_addr), port_n);
			
			pid = fork();
			if(pid < 0)
			{
				perror("fork");
				exit(7);
			}
			else if (pid == 0)
			{
				close(sockt);
				if(service_is_active)
				{
					sprintf(line_out, "**LIBRARY SERVER**.\nSERVER: insert command (L, P, R, F).\n");
					write(msgsockt, &line_out, strlen(line_out)+1);
					do
					{
						read(msgsockt, &line_in, sizeof(line_in));
						char cmd;
						int id_b, cde;
						sscanf(line_in, "%s %d %d", &cmd, &id_b, &cde);
						if (cmd == 'L')
						{
							sprintf(line_out, "SERVER: available books in library:\n");
							write(msgsockt, &line_out, strlen(line_out)+1);
							for(int i = 0; i < book_n; i++)
							{
								book bk;
								read(piped[i][0], &bk, sizeof(bk));
								if (bk.available == TRUE)
								{
									sprintf(line_out, "	%s\n", bk.title);
									write(msgsockt, &line_out, strlen(line_out)+1);
								}
								write(piped[i][1], &bk, sizeof(bk));	
							}	
						}
						else if(cmd == 'P')
						{
							for(int i = 0; i < book_n; i++)
							{
								book bk;
								read(piped[i][0], &bk, sizeof(bk));
								if (bk.book_id == id_b && bk.available == TRUE)
								{
									sprintf(line_out, "SERVER: book %d available.\n", bk.book_id);
									write(msgsockt, &line_out, strlen(line_out)+1);
									bk.available = FALSE;
									srand(time(NULL));
									cde = rand()%89999 +10000;
									bk.code = cde;
									sprintf(line_out, "SERVER: OK %d.\n", cde);
									write(msgsockt, &line_out, strlen(line_out)+1);
									
								}
								else if (bk.book_id == id_b && bk.available == FALSE)
								{
									sprintf(line_out, "SERVER: book %d not available.\n", bk.book_id);
									write(msgsockt, &line_out, strlen(line_out)+1);
								}
								write(piped[i][1], &bk, sizeof(bk));	
							}
						}
						else if(cmd == 'R')
						{
							for(int i = 0; i < book_n; i++)
							{
								book bk;
								read(piped[i][0], &bk, sizeof(bk));
								if (bk.book_id == id_b && bk.code == cde)
								{
									sprintf(line_out, "SERVER: book %d given back.\n", bk.book_id);
									write(msgsockt, &line_out, strlen(line_out)+1);
									bk.available = TRUE;
								}
								else if (bk.book_id == id_b && bk.code != cde)
								{
									sprintf(line_out, "SERVER: wrong code for book %d.\n", bk.book_id);
									write(msgsockt, &line_out, strlen(line_out)+1);
								}
								write(piped[i][1], &bk, sizeof(bk));	
							}
						}
						
						else if(cmd == 'F')
						{
							sprintf(line_out, "SERVER: session ended.\n");
							write(msgsockt, &line_out, strlen(line_out)+1);
							close(msgsockt);
							exit(0);
						}
						
						
						
					} while(1);
				}
				else
				{
					sprintf(line_out, "SERVER: sorry, service is currently suspended.\n");
					write(msgsockt, &line_out, strlen(line_out)+1);
					close(msgsockt);
					exit(0);
				}
			}
			else
			{
				//close(sockt);
				close(msgsockt);
			}
			
			
		}
	
	
	
	
	
	
	
	} while(1);
	
	exit(0);
}




