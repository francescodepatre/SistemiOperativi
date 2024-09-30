#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define MAXBUF 8192

int main(int argc, char * argv[])
{
    int client_socket, connect_socket, portnum;
    int fd, retcode, readed;

    struct sockaddr_in server;
    struct hostent *server_addr;

    char message[MAXBUF];

    char *hostname, *filename;

    if(argc != 4)
    {
        printf("Uso:\n%s nomefile nomehost portno\n",argv[0]);
        exit(1);
    }

    filename = argv[1];
    hostname = argv[2];
    printf("Client (%d): initializing\n", getpid());

    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1)
    {
        perror("Socket error");
        return(2);
    }

    portnum = atoi(argv[3]);
    server.sin_family = AF_INET;
    server.sin_port = htons(portnum);
    server_addr = gethostbyname(hostname);
    if (server_addr == NULL)
    {
        fprintf(stderr, "ERROR: unknown host\n");
        exit(3);
    }
    memcpy((char *)&server.sin_addr,(char *)server_addr->h_addr_list[0],server_addr->h_length);
    //memcpy((char *)&server.sin_addr,(char *)server_addr->h_addr,server_addr->h_length);

    retcode = connect(client_socket,
                      (struct sockaddr *)&server,
                      sizeof(server));

    if (retcode == -1)
    {
        perror("Connection error");
        exit(4);
    }
    else
    {
        printf("Client (%d): connected to server\n", getpid());
    }

    fd = open(filename, O_RDONLY);
    if (fd == -1)
    {
        perror("File opening error");
        exit(5);
    }

    do
    {
        readed = read(fd, message, MAXBUF);
        if (readed > 0)
        {
            retcode = write(client_socket, message, readed);
            if (retcode == -1)
            {
                perror("Write error");
                exit(6);
            }
            
        }
        
    } while (readed > 0);
    

    printf("Client (%d): file %s succesfully sent\n", getpid(), filename);
    close(fd);
    printf("File: closing");
    close(client_socket);
    printf("Socket: closing");
    exit(0);
}