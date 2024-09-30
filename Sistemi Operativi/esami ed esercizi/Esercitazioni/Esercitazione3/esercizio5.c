#include<stdlib.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<fcntl.h>
#include<unistd.h>
#include<stdio.h>
#include<string.h>


#define N 256

int main(int argc, char **argv) {
	int nread, nwrite =0, atteso, status, fileh, pid;
	char st1[N];
	char st2[N] ="";

	fileh = open(argv[1], O_CREAT| O_RDWR | O_TRUNC, 0644);
	if(fileh ==-1){
		perror("open error");	
		exit(-1);
	}
	
	if((pid =fork()) <0){
		perror("fork error");
		close(fileh);
		exit(-2);
	}

	if(pid ==0){
		read(0,st1,N);	
		nwrite = write(fileh,st1,strlen(st1));
		if(nwrite==-1){
			perror("write error");
		}
		exit(0);
	}

	
	else{
		atteso = wait(&status);
		lseek(fileh, 0, SEEK_SET);
		nread = read(fileh, st2, N);
		if(nread ==-1){
			perror("read error");
		}
		printf("nread = %d\n" , nread);	
		printf("Il figlio ha scritto la stringa: %s\n" , st2);
		close(fileh);
		close(0);
		return(0);
}
exit(0);
}	

	
















