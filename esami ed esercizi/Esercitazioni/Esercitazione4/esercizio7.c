#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>



int main(){
	int pid,j,c;
	int piped[2];
	
	/*Crea la pipe ottenendo due file descrpitor uno per la lettura e uno per la scrittura e vengono memorizzati nell'array piped[]*/
	
	if(pipe(piped)<0){
		exit(-1);
	}
		
	if((pid =fork())<0){
		exit(-2);}
	
	if(pid==0){
		close(piped[1]);
		for(j =1; j<=10; j++){
			read(piped[0], &c , sizeof(int));
			printf("Figlio: ho letto dala pipe il numero %d e il mio pid è %d\n" , c , getpid());
		}
	exit(0);
	}
	else{
		close(piped[0]);
		for(j=1;j<=10;j++){	
			write(piped[1], &j , sizeof(int));
			sleep(1);
		}
	exit(0);
}}
