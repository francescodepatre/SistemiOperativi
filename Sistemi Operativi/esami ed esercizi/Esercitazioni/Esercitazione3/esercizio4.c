#include<unistd.h>
#include<sys/types.h>
#include<stdlib.h>
#include<stdio.h>

int main()
{
	pid_t pid;
	if((pid =fork())<0){
		perror("Fork() error");	
		exit(-1);
	}
	fork();
	fork();
	fork();
	printf("Ciao, pid vale %d\n", getpid());
	exit(0);

}
