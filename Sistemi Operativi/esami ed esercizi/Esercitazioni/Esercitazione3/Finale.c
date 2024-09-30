#include<sys/wait.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<stdio.h>
#include<errno.h>

#include<string.h>
#define N 5

int main(int argc , char *argv[]){
	
	
	int status;
	char *args[N];
	char com[90]="/bin/";
	strcat(com, argv[1]);
	printf("%s\n",com);
	int i;
	char *env[] = {"TERM=vt100", "PATH=/bin:/usr/bin", (char *) 0 };
	for(i=0;i<argc;i++){
		args[i]= argv[i+1];}
	args[i+1]=(char *) 0;
	int pid =fork();

	if(pid<0){
		perror("Fork error");
		exit(-1);
	}
	
	
	if(pid==0){	
		int ret = execve(com,args,env);
		if(ret<0){
		fprintf(stderr, "Non va perche %s\n" ,strerror(errno));}
			
		exit(0);
	}
	else
	{
		
		wait(&status);
		printf("exit di %d con %d\n", pid, WEXITSTATUS(status));
	}
		
	exit(0);
}
