#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>




int main(int argc, char *argv[])
{
  int sock, nuovasock;
  struct sockaddr_in nomesocket;
  char buf[BUFSIZ];
  FILE *lfp;

  /* Controllo degli argomenti */
  if (argc != 2)
    {
      printf("Uso: %s <numero porta>\n", argv[0]);
      exit(1);
    }

  /* Crea la socket */
  sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0)
    {
      perror("creazione stream socket");
      exit(2);
    }
  
  /* Prepara le informazioni per il binding */
  nomesocket.sin_family = AF_INET;
  nomesocket.sin_addr.s_addr = INADDR_ANY;
  nomesocket.sin_port = htons(atoi(argv[1]));
  
  /* Collega la socket alla porta specificata */
  if (bind(sock, (struct sockaddr *) &nomesocket, sizeof(nomesocket)) < 0)
    {
      printf("bind su stream socket");
      exit(3);
    }
  
  /* Ascolta le richieste di connessione sulla porta */
  if (listen(sock, 3) < 0)
    {
      perror("listen error");
      exit(4);
    }
  do 
    {
      /* Accetta una delle connessioni pendenti */
      nuovasock = accept(sock, 0, 0);  


      /* Redirezione dello stdin sulla socket */

      close(0);
      dup(nuovasock);

      /* Mostra i messaggi che riceve dal client*/
      while(buf[0]!= 0x0d && buf[0]!=0x0a )
	{
	  fgets(buf,128,stdin);
	  fprintf(stderr,"%s",buf);
	}


      /* Ricava la lista dei file contenuti nel direttorio corrente */

      system("ls > lista"); 
      /* Apre listfile in lettura */ 
      lfp = fopen("lista", "r");

      /* Crea e invia pagina HTML */
      //strcpy(buf,"HTTP/1.0 200 OK\n\n");
      strcpy(buf, "<html><head><title> Lista </title></head>\n");
      strcat(buf, "<body>\n<h2>File contenuti nel direttorio del server:</h2>\n<ul>\n");
      write(nuovasock, buf, strlen(buf));
      while (fgets(buf, BUFSIZ, lfp) != NULL)
	{
	  write(nuovasock, "<li>", 4); 
	  write(nuovasock, buf, strlen(buf));
	}
      fclose(lfp);
      write(nuovasock, "</ul>\n", 7);
      strcpy(buf,"</body>\n</html>\n\n");
      write(nuovasock, buf, strlen(buf));
      
      /* Chiude il canale di comunicazione con il client*/
      shutdown(nuovasock,1);
      close(nuovasock);
      
    }
  while(1);
}
