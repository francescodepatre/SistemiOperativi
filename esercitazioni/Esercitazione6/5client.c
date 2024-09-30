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
    clock_t start, end;
    int port_n, lenght;
    int retcode, scktname;
    int sockt, snd, rcv;
    unsigned int server_lenght;
    struct sockaddr_in server, client;
    struct hostent *server_addr;
    char in_buffer[BYTES_NR], out_buffer[BYTES_NR];
    char *hostname;
    double duration;

    if(argc !=3)
    {
        fprintf(stderr,"Usage: %s servername serverport\n",argv[0]);
        exit(1);
    }

    sockt = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockt < 0)
    {
        perror("creating socket");
        exit(1);
    }

    port_n = atoi(argv[2]);
    

    client.sin_family = AF_INET;
    client.sin_addr.s_addr = INADDR_ANY;
    client.sin_port = htons(0);

    lenght = sizeof(client);

    retcode = bind(sockt, (struct sockaddr *)&client, lenght);
    if (retcode < 0)
    {
        perror("bind");
        exit(2);
    }

    scktname = getsockname(sockt, (struct sockaddr *)&server, &lenght);
    if (scktname < 0)
    {
        perror("getting socket name");
        exit(3);
    }
    
    hostname = argv[1];

    server.sin_family = AF_INET;
    server.sin_port = htons(port_n);
    server_addr = gethostbyname(hostname);
    if (server_addr == 0)
    {
        fprintf(stderr, "%s: host is unknown", hostname);
        exit(3);
    }

    memcpy((char *)&server.sin_addr,(char *)server_addr->h_addr_list[0],server_addr->h_length);

    strcpy(in_buffer,"hello");

    //server_lenght = sizeof(server);

    start = clock();
    sendto(sockt, in_buffer, sizeof(in_buffer), 0, (struct sockaddr*)&server, sizeof(server));
    printf("Message sent\n");
    recvfrom(sockt, out_buffer, sizeof(out_buffer), 0, (struct sockaddr*)&server, sizeof(server));
    end = clock();
    printf("Message received\n");
    duration = ((double)end - start)/CLOCKS_PER_SEC;

    close(sockt);
    printf("Time taken to send message in seconds : %f", duration);

    
    exit(0);
    
}