#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include<errno.h>
#include<string.h>
#include<stdio.h>
#define PERM 0644
#define BUFSIZ 4096


int main(int argc, char **argv)

{
char *f1= argv[1], *f2= argv[2] , buffer[BUFSIZ];
int infile, outfile; /* file descriptor */
int nread;
if(argc !=3)
{
	fprintf(stderr, "Uso %s nomefilesorgente nomefiledestinazione" , argv[0]);
	exit(-1);
}

/* apertura file sorgente */
if ((infile=open(f1,O_RDONLY)) <0)
{ 
	fprintf(stderr, "Il file %s non è accessibile: %s\n", argv[1], strerror(errno));
	//perror("Apertura f1: ");
	exit(-2);
}
/* creazione file destinazione */
if ((outfile=open(f2, O_CREAT|O_WRONLY|O_TRUNC, 0644)) <0)
{ 	
	fprintf(stderr, "Il file %s non può essere creato: %s\n" , argv[2],strerror(errno));
	//perror("Creazione f2: ");
	exit(-3);
}
/* Ciclo di lettura/scrittura fino alla fine del file sorgente */
while((nread= read(infile, buffer, BUFSIZ)) >0){
	/* if(write(outfile, buffer, nread) != nread)
		{
			perror("Errore write: ");
 			exit(-3);
		}
*/	
	write(outfile,buffer,nread);
}
if(nread < 0) 
{
	perror("Errore read: ");
	exit(-4);
}

return 0;
}


