#include<sys/types.h>
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<sys/wait.h>

int main(){
	pid_t childPIDorZero = fork();
	if(childPIDorZero <0){
		perror("Fork() error" );
		exit(-1);
	}
	if(childPIDorZero!=0){
		printf("Sono il padre %d, mio figlio è %d\n", getpid(), childPIDorZero);
		wait(NULL); //aspetta il figlio
	}
	else {
	printf("Sono il figlio %d, mio pdre è %d\n" , getpid(), getppid());
	}
return 0;
}
