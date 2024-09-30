#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include<errno.h>
#include<string.h>
#include<stdio.h>
#include<sys/types.h>
#define REC_LEN 17

#define N 4
int main() {
int f;
long int length, cur_pos;
char buf[REC_LEN];
if((f = open("test.dat", O_RDWR)) < 0)
{
	perror("Apertura di test.dat impossibile");
	exit(-1);
}
printf("This file has %ld bytes\n", length = lseek(f, 0, SEEK_END));
/* Si riposiziona all’inizio */
cur_pos = lseek(f, 0, SEEK_SET);
/* Il file contiene dei record di REC_LEN Byte;
ne viene letto uno ogni 5 */
while (cur_pos < length) {
	int i = cur_pos;
	read(f, buf, N);
	write(1, buf, N);
	cur_pos = lseek(f,REC_LEN-N+1, SEEK_CUR);
}

close(f);
return 0;
}
