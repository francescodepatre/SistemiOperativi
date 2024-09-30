#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h> 
#include <string.h>
#include <fcntl.h>


#define MAXBUF     8192

int main(int argc, char * argv[])
{
  int server_socket,connect_socket,portno;
  unsigned int client_addr_len;
  int retcode,fd;
  struct sockaddr_in client_addr, server_addr;
  char line[MAXBUF];

  if(argc!=3) 
  {
    printf("Usage:\n%s port nomefilelocale\n",argv[0]);
    return(0);
  }
  printf("Server: fase di inizializzazione\n");
  server_socket = socket(AF_INET,SOCK_STREAM,0);
  if(server_socket == -1) 
  { 
    perror("aprendo il socket del server"); 
    return(-1);
  }

  portno = atoi(argv[1]);
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port   = htons(portno);

  retcode = bind(server_socket, 
                 (struct sockaddr *)&server_addr,
                 sizeof(server_addr));
  if(retcode == -1) 
  { 
    perror("bind error");
    return(-1); 
  }
  listen(server_socket,1); 
  printf("Server: attendo connessione\n");
  client_addr_len = sizeof(client_addr);
  connect_socket = accept(server_socket,
                          (struct sockaddr *) & client_addr,
                          &client_addr_len);
  printf("Server: accettata nuova connessione\nApro file locale %s",argv[2]);
  fd = open(argv[2],O_WRONLY|O_TRUNC|O_CREAT,0644);
  if(fd == -1) 
  {
    perror("aprendo il file locale");
    return(-2);
  }
  do {
    retcode = read(connect_socket,line,MAXBUF);
    if(retcode != -1) 
      write(fd,line,retcode);
  } while(retcode > 0);
  close(fd);
  printf("\nFine del messaggio, chiusura della connessione\n");
  close(connect_socket);

  printf("Chiusura dei lavori ... \n");
  close(server_socket);
  return(0);
}
