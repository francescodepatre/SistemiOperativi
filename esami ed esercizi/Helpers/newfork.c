#include<sys/types.h>
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<sys/wait.h>


int main(){
int childpid;
int count1=0, count2 =0;
printf("Prima della fork()\n");

sleep(5);
childpid = fork();

if(childpid <0){
	perror("fork() error");
	exit (-1);
}

if(childpid ==0){
printf("Questo è il processo figlio con il pid: %d\n" ,getpid());
while(count1<10)
{
	printf("Processo figlio: %d\n", count1);
	sleep(1);
	count1++;
}
}


else{
printf("Questo è il processo padre con il pid: %d\n" , getppid());
while(count2<20)
{
	printf("Processo padre: %d\n", count2);
	sleep(1);
	count2++;
}
}
return 0;
}

