#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#define BUFSIZ 4096
main(int argc, char **argv)

{
char
*f1 = argv[1],*f2=argv[2] , buffer[BUFSIZ];
int
infile, outfile; /* file descriptor */
int
nread;
/* apertura file sorgente */
if ((infile=open(f1,O_RDONLY)) <0)
{ perror("Apertura f1: "); exit(-1); }
/* creazione file destinazione */
if ((outfile=open(f2,O_WRONLY|O_CREAT|O_TRUNC, 0644)) <0)
{ perror("Creazione f2: "); exit(-2);}
/* Ciclo di lettura/scrittura fino alla fine del file sorgente */
while((nread= read(infile, buffer, BUFSIZ)) >0)
if(write(outfile, buffer, nread) != nread)
{ perror("Errore write: "); exit(-3);}
if(nread < 0) { perror("Errore read: "); exit(-4);}
close(infile); close(outfile);
return 0;
}
