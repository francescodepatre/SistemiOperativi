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
    int conct;
    int portnum, lenght, req, answ;
    struct sockaddr_in server, sockaddr_in;
    struct hostent *server_addr;
    char *hostname;

    srand(time(NULL));

    if (argc != 3)
    {
        printf("Use: %s localhost <port number>\n", argv[0]);
        exit(1);
    }
    

    sockt = socket(AF_INET, SOCK_STREAM, 0);
    if (sockt < 0)
    {
        perror("creating socket");
        exit(2);
    }

    portnum = atoi(argv[2]);
    hostname = argv[1];

    server.sin_family = AF_INET;
    server.sin_port = htons(portnum);
    server_addr = gethostbyname(hostname);
    if (server_addr == 0)
    {
        fprintf(stderr, "%s: host is unknown", hostname);
        exit(3);
    }

    memcpy((char *)&server.sin_addr,(char *)server_addr->h_addr_list[0],server_addr->h_length);
    
    printf("Connecting to %s on port # %d...\n", hostname, portnum);
    conct = connect(sockt, (struct sockaddr *)&server, sizeof(server));
    if (conct < 0)
    {
        perror("connecting stream socket");
        exit(4);
    }
    printf("Connected!\n");

    req = rand()%50;

    write(sockt, &req, sizeof(req));
    printf("Request # %d sent\n", req);

    read(sockt, &answ, sizeof(answ));

    printf("Answer from %s: %d\n", hostname, answ);

    close(sockt);
    exit(0);
}