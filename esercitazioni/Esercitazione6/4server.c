#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

int main(int argc, char *argv[])
{
    int sockt, msgsockt;
    int retcode, sockname;
    int port_n, lenght, pid;
    struct sockaddr_in server, client;
    unsigned int client_lenght;
    char buffer[26];
    time_t ticks;

    printf("SERVER: current date and time courier to clients\n");
    printf("Press ^C to exit program\n");

    if (argc != 2)
    {
        printf("Use: %s <port number>\n", argv[0]);
        exit(1);
    }

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
    
    retcode = bind(sockt, (struct sockaddr *)&server, lenght);
    if (retcode < 0)
    {
        perror("binding stream socket");
        exit(3);
    }

    sockname = getsockname(sockt, (struct sockaddr *)&server, &lenght);
    if (sockname < 0)
    {
        perror("getting socket name");
        exit(4);
    }

    listen(sockt, 2);
    printf("Server: listening on port #%d\n", htons(server.sin_port));

    do
    {
        client_lenght = sizeof(client);
        msgsockt = accept(sockt, (struct sockaddr *)&client, &client_lenght);
        printf("\nConnecting to %s to sent data\n", inet_ntoa(client.sin_addr));
        if (msgsockt < 0)
        {
            perror("accept");
            exit(5);
        }
        else
        {
            pid = fork();
            if (pid < 0)
            {
                perror("creating child process");
            }
            else if (pid == 0)
            {
                printf("connected!\n");
                ticks = time(NULL);
                strcpy(buffer,ctime(&ticks));
                printf("Current date and time: %s", buffer);
                write(msgsockt, &buffer, sizeof(buffer));
                printf("data sent!\n");
                close(msgsockt);
                exit(0);
            }
            else
            {
                close(msgsockt);
            }
        }
    } while (1);

    exit(0);
}