#include<signal.h>
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<time.h>

void handler(int signo){
	static int beeps =0;
	printf("Beep\n");
	if(++beeps<5){
		alarm(1);
	}	
	else{
		printf("BOOM\n");
		exit(0);}
	}

int main()
{
	struct timespec ts;
	signal(SIGALRM, handler);	
	alarm(1);
	
	ts.tv_sec =20;
	ts.tv_nsec = 0;
	
	while(1){
		nanosleep(&ts, NULL);}
	exit(0);
}
