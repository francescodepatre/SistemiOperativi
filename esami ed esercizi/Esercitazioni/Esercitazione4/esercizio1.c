#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<signal.h>

void catchint(int signo){	
	printf("Catchint: signo = %d \n" , signo);
}

int main(){
	//signal(SIGINT, catchint);
	signal(SIGTSTP,catchint);
	for(;;)
	{
		printf("In attesa del segnale SIGTSTP (premere ctrl^z)\n");
		sleep(1);
	}
	exit(0);
}
