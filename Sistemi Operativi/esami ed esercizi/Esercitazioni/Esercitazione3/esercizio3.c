#include<sys/types.h>
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<sys/wait.h>


int main() {
int pid, status;
int myvar;
myvar =1;
if((pid=fork())<0){
perror("fork() error");
exit(-1);}

if(pid ==0){
	sleep(5);
	myvar =2;
	printf("Figlio: sono il processo %d e sono figlio di %d\n" ,getpid() , getppid());
	printf("Figlio: myvar = %d\n" , myvar);
	exit(0);
}

else{
	wait(&status);
	printf("Padre: sono il processo %d e sono figlio di %d \n", getpid(), getppid());
	printf("Padre: status = %d \n" , status);
	printf("Padre: myvar = %d\n" , myvar);
	}
return 0;
}

