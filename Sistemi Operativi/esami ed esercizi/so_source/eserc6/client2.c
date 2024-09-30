#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

#define MAXBUF     8192

int main(int argc, char * argv[])
{
  int client_socket,fd,portno;
  int retcode,letti;
  struct sockaddr_in  server_addr;
  char message[MAXBUF];
  char *nomehost,*filename;

  if(argc != 4) 
  {
    printf("Usage:\n%s nomefile nomehost portno\n",argv[0]);
    return(0);
  }
  filename = argv[1];
  nomehost = argv[2];
  printf("Client (%d): fase di inizializzazione\n",getpid());
  client_socket = socket(AF_INET,SOCK_STREAM,0);
  if(client_socket == -1) 
  { 
    perror("aprendo il socket del cliente"); 
    return(-1);
  }

  portno = atoi(argv[3]);
  server_addr.sin_family = AF_INET;
  server_addr.sin_port   = htons(portno);
  memcpy(&server_addr.sin_addr,(gethostbyname(nomehost)->h_addr),
	 sizeof(server_addr.sin_addr));
  retcode = connect(client_socket,
		 (struct sockaddr *)&server_addr,
		 sizeof(server_addr));
  if(retcode == -1) 
  { 
    perror("connettendo il socket");
    return(-1); 
  }
  fd = open(filename,O_RDONLY);
  if(fd == -1) 
  {
    perror("aprendo il file");
    return(-3);
  }
  do {
    letti = read(fd,message,MAXBUF);
    if(letti > 0) { /* solo se la lettura ha avuto buon fine */
      retcode = write(client_socket,message,letti);
      if(retcode == -1) 
      { 
	perror("scrivendo il messaggio"); 
	return(-3); 
      }
    }
  } while (letti > 0);
  close(fd);
  close(client_socket);
  return(0);
}
