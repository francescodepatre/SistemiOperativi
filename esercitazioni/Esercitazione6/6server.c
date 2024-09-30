#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char* argv[])
{
    int sockt, on, portnumber, lenght, retcode, nm;
    int sockfd, pid, msg_sockt, counter_value, value;
    struct sockaddr_in server, client;
    char buffer[256], answer[256];
    int pipecount[2], counter;
    char *hellomsg = "Insert integer number to add to counter\n";

    if (argc != 2)
    {
        printf("Use: %s <port number>", argv[0]);
        exit(1);
    }
    
    /*creazione e controllo socket*/
	sockt = socket(AF_INET,SOCK_STREAM,0);
    if (sockt < 0)
    {
        perror("creating socket");
        exit(2);
    }
    
    portnumber = atoi(argv[1]);

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(portnumber);

    retcode = bind(sockt, (struct sockaddr *)&server, sizeof(server));
    if (retcode < 0)
    {
        perror("making socket bind");
        exit(3);
    }
    
    lenght = sizeof(server);

    nm = getsockname(sockt, (struct sockaddr *)&server, &lenght);
    if(nm < 0)
    {
        perror("getting socket name");
        exit(4);
    }

    /* creo la pipe*/

    if(pipe(pipecount) < 0)
    {
        perror("creating pipe");
        exit(5);
    }

    counter = 0;
    write(pipecount[1], &counter, sizeof(counter));

    /*accettazione dei messaggi*/

    listen(sockt, 1);
    printf("SERVER: in ascolto sulla porta # %d\n",ntohs(server.sin_port));

    do
    {
        msg_sockt = accept(sockt, (struct sockaddr *)&client, (socklen_t *)&lenght);
        if (msg_sockt == -1)
        {
            perror("accept");
        }
        else
        {
            printf("Connection from %s, port # %d", inet_ntoa(client.sin_addr), htons(client.sin_port));

            /*server concorrente*/
            pid = fork();
            if (pid == 0)
            {
                write(msg_sockt, hellomsg, strlen(hellomsg)+1);
                read(msg_sockt, &buffer, sizeof(buffer));
                sscanf(buffer, "%d", &value);

                read(pipecount[0], &counter_value, sizeof(counter_value));
                counter_value += value;
                write(pipecount[1], &counter_value, sizeof(counter_value));

                sprintf(answer, "New counter: %d\n", counter_value);

                write(msg_sockt, answer, strlen(answer)+1);
                close(msg_sockt);
                exit(0);
            }
            else
            {
                if (pid < 0)
                {
                    perror("fork");
                    exit(6);
                }
                else
                {
                    close(msg_sockt);
                }
            }
        }
    } while (1);
    
    exit(0);
}