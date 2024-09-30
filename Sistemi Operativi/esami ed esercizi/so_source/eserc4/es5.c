#include <stdio.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>


int main (int argc, char **argv) 
{
  int nread, nwrite = 0, atteso, status, fileh, pid;
  char st1[256];
  char st2[256];

 if(argc != 2) 
	{
	fprintf(stderr,"Uso: %s nomefile\n",argv[0]);
	exit(-1);
	}


  /* APERTURA IN LETTURA/SCRITTURA */
  if( (fileh=open(argv[1], O_CREAT|O_RDWR|O_TRUNC, 0644))<0)
	{
	perror("open error: ");
	exit(-1);
	}
  
  if((pid=fork()) < 0)
    {
      perror("fork error");
      close(fileh);
      exit(-1);
    }
  else
    if (pid==0)
      {
	printf("Processo figlio: introdurre una stringa : \n");
	scanf("%s", st1);
	nwrite=write(fileh, st1, strlen(st1)+1); /* per scrivere anche il \0 terminatore della stringa */
	printf("Processo figlio: termino con esito=%d\n",nwrite);
	exit(nwrite);
      }
  else
    {
      atteso=wait(&status); /* ATTESA DEL FIGLIO */
      printf("Processo padre: il figlio con PID=%d e' terminato con stato=%d\n",atteso,WEXITSTATUS(status));
      lseek(fileh, 0, SEEK_SET);
      nread=read(fileh, st2, 256);
      printf("Processo padre: il figlio ha scritto la stringa %s\n", st2);
      close(fileh);
      return(0);
  }

  exit(0);
}
