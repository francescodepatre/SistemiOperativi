#include <sys/socket.h>
#include <sys/types.h>
#include <sys/timeb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#define BYTES_NR 64

int main(int argc, char *argv[])
{
    int port_n, lenght, pid;
    int retcode, scktname, rval, sval;
    int sockt, msgsock;
    unsigned int client_lenght;
    struct sockaddr_in server, client;
    char buffer[BYTES_NR];

    if (argc != 2)
    {
        printf("Use: %s <port_number>\n", argv[0]);
        exit(1);
    }
    
    sockt = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockt < 0)
    {
        perror("creating socket");
        exit (2);
    }
    
    port_n = atoi(argv[1]);

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port_n);

    lenght = sizeof(server);

    retcode = bind(sockt, (struct sockaddr *)&server, lenght);
    if (retcode < 0)
    {
        perror("bind");
        exit(3);
    }
    
    scktname = getsockname(sockt, (struct sockaddr *)&server, &lenght);
    if (scktname < 0)
    {
        perror("getting socket name");
        exit(4);
    }

    while (1)
    {
        do
        {
            bzero(buffer, sizeof(buffer));
            client_lenght = sizeof(client);
            rval = recvfrom(sockt, buffer, sizeof(buffer), 0, (struct sockaddr *)&client, &client_lenght);
            if (rval < 0)
            {
                perror("reading stream message");
            }
            else if (rval == 0)
            {
                printf("Ending connection\n");
            }
            else
            {
                printf("\nReceived message '%s' from %s on port # %d\n", buffer, inet_ntoa(client.sin_addr), port_n);
                strcat(buffer, "*");
                printf("Sending back message '%s'\n", buffer);
                sval = sendto(sockt, buffer, sizeof(buffer), 0, (struct sockaddr *)&client, client_lenght);
                if (sval < 0)
                {
                    perror("sending message");
                } 
                printf("Message sento to %s\n", inet_ntoa(client.sin_addr));
            }
        } while (rval != 0);
    
    }
    exit(0);    
}