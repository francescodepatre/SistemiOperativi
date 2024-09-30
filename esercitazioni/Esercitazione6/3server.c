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


int main (int argc, char* argv[])
{
    int sockt, msgsockt;
    int sockfd, scktname, retcode;
    int port_n, lenght, pid, request, answer, diff;
    char buffer[256];
    struct sockaddr_in server, client;

    //srand(time(NULL)); 

    if (argc != 2)
    {
        printf("use: %s <port number>", argv[0]);
        exit(1);
    }
    
    /*socket creation*/
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

    retcode = bind(sockt, (struct sockaddr*)&server, lenght);
    if (retcode < 0)
    {
        perror("binding stream socket");
        exit(3);
    }
    
    scktname = getsockname(sockt, (struct sockaddr *)&server, &lenght);
    if (scktname < 0)
    {
        perror("getting socket name");
        exit(4);
    }
    
    listen(sockt, 2);
    printf("Server: listening on port #%d\n", htons(server.sin_port));

    do
    {
        msgsockt = accept(sockt, (struct sockaddr *)&client, (socklen_t *)&lenght);
        if (msgsockt < 0)
        {
            perror("accept");
            exit(5);
        }
        else
        {
            printf("Connection from %s, port %d\n", inet_ntoa(client.sin_addr), htons(client.sin_port));

            pid = fork();
            if (pid < 0)
            {
                perror("fork");
                exit(6);
            }
            else if (pid == 0)
            {
                /*child code*/
                srand(time(NULL));
                close(sockt);
                read(msgsockt, &request, sizeof(request));
                printf("Received request # %d from %s\n", request, inet_ntoa(client.sin_addr));
                diff = rand() %100;
                answer = request + diff;
                printf("Add number: %d\n", diff);
                printf("Sending answer # %d\n", answer);
                write(msgsockt, &answer, sizeof(answer));
                close(msgsockt);
                exit(0);
            }
            else
            {
                /*parent code*/
                close(msgsockt);
            }
        }
    } while (1);
    
    exit(0);
}