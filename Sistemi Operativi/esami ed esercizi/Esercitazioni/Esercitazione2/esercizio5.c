#include <unistd.h>
#include<stdio.h>
#include<fcntl.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>

#define PERM 0644

int main()
{
int fd;
printf("Sto scrivendo sullo stdout\n");
fd=open("file.txt", O_CREAT| O_WRONLY | O_TRUNC, PERM);
if(fd ==-1)
{
    perror("open error ");
    exit(-1);
}

close(1); //chiude lo standard output
dup2(fd,1);

write(1, "Hello from write dup2\n", 20);
printf("Hello from printf dup2\n");

return 0;

}
