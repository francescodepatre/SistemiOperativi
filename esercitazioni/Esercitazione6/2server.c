#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#define MAXBUFF 8192

int main(int argc, char *argv[])
{
    int server_socket, connect_socket, portnum;
    unsigned int client_add_len;
    int retcode, fd;
    char line[MAXBUFF];

    struct sockaddr_in client_addr, server_addr;

    if (argc != 3)
    {
        printf("Needed: %s port nomefilelocale\n", argv[0]);
        return(1);
    }

    printf("Server: initializing\n");
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1)
    {
        perror("Socket error");
        return(2);
    }
    
    portnum = atoi(argv[1]);

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(portnum);

    retcode = bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (retcode == -1)
    {
        perror("Bind error");
        exit(3);
    }

    listen(server_socket, 1);
    printf("Server: waiting for connection on port n°: %d\n", portnum);
    client_add_len = sizeof(client_addr);
    connect_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_add_len);
    if (connect_socket < 0)
    {
        printf("Accept error");
        exit(4);
    }
    
    printf("Server: connection accepted\nOpening local file: %s\n", argv[2]);
    fd = open(argv[2], O_WRONLY | O_TRUNC | O_CREAT, 0644);
    if (fd == -1)
    {
        perror("File opening error");
        return(5);
    }
    
    do
    {
        retcode = read(connect_socket, line, MAXBUFF);
        if (retcode != -1)
        {
            write(fd, line, retcode);
        }
        
    } while (retcode > 0);

    close(fd);
    printf("File: closing\n");
    close(connect_socket);
    printf("Connection: closing\n");
    close(server_socket);
    printf("Socket: closing\n");
    exit(0);
}
