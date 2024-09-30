#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>


int main (int argc, char **argv) 
{
  int nread, nwrite = 0, atteso, status, fileh, pid;
  char st1[256];
  char st2[256];

 if(argc != 2) 
	{
	fprintf(stderr,"Uso: %s nomefile\n",argv[0]);
	exit(1);
	}


  /* APERTURA IN LETTURA/SCRITTURA */
  fileh=open(argv[1], O_CREAT|O_RDWR|O_TRUNC, 0644);
  if(fileh<0) {
	  perror("Errore open");
	  exit(2);
  }
	  
  
  if((pid=fork()) < 0)
    {
      perror("Errore fork");
      exit(3);
    }
  else
    if (pid==0) {
        printf("Scrivere una stringa (senza spazi) e premere Invio\n");
        scanf("%s", st1);
        nwrite=write(fileh, st1, strlen(st1)+1);
        exit(0);
        }
  else  {
        atteso=wait(&status); /* ATTESA DEL FIGLIO */
        printf("Il figlio con PID=%d e' terminato con stato=%d\n",atteso,WEXITSTATUS(status));
        lseek(fileh, 0, SEEK_SET); // il file offset è riposizionato all'inizio del file
        nread=read(fileh, st2, 256);
        printf("Il figlio ha scritto la stringa %s\n", st2);
        close(fileh);
        return(0);
        }

  exit(0);
}
