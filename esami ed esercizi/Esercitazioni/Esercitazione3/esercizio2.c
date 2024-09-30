#include<sys/types.h>
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<sys/wait.h>

int main(){
int pid;
if((pid =fork()) <0){
perror("fork() error");
exit(-1);
}
if(pid ==0){
printf("Sono il processo figlio con PID %d\n" , getpid());
}
else{
printf("Sono il processo padre con PID %d e ho generato un figlio con PID %d\n", getpid(), pid);
}
return 0;
}
