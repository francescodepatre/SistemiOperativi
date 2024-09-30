#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>




#define perm 0744
#define N 2048

int main(int argc, char **argv)
{
  int infile, outfile, nread;
  char buffer[N];


/* Controllo del numero degli argomenti */
if (argc != 3) 	
	{ fprintf(stderr,"Uso: %s filesorg filedest\n",argv[0]);
	exit(-1); }

  infile = open(argv[1], O_RDONLY);
  outfile = creat(argv[2], perm);
  /* Un altro modo di creare il file di output:                */
  /* outfile = open(argv[2], O_CREAT|O_WRONLY|O_TRUNC, perm);  */
  
  while ((nread = read(infile, buffer, N)) > 0)
    write(outfile, buffer, nread);
  
  close(infile);
  close(outfile);

  return 0;
}
