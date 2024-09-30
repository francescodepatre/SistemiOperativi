#include <sys/stat.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <time.h>

#define perm 0744

int main(void)
{
  struct stat statbuf;
  int file;

  /* crea un file*/
  file = creat("dummy.fil", perm);
  if (file == -1)
    perror("file creation error");

  /* ottiene informazioni sul file */
  fstat(file, &statbuf);
  close(file);

  /* mostra le informazioni ottenute */
  if (statbuf.st_mode & S_IFCHR) 
    printf("Handle refers to a device.\n");
  if (statbuf.st_mode & S_IFREG)
    printf("Handle refers to an ordinary file.\n");
  if (statbuf.st_mode & S_IREAD)
    printf("User has read permission on file.\n");
  if (statbuf.st_mode & S_IWRITE)
    printf("User has write permission on file.\n");

printf("Size of file in bytes: %ld\n", statbuf.st_size);
  printf("Time file last opened: %s\n", ctime(&statbuf.st_ctime));

  return 0;
}
