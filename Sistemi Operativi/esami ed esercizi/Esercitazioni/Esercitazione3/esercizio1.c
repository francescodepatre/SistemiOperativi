
#include<sys/types.h>
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<sys/wait.h>


int main(){
printf("Sono il processo %d e sono figlio di %d\n" , getpid(), getppid());
return 0;
}
