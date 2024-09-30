#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

int main() {
	
	int ret;
	printf("Ho chiamato il programma prova_execl\n");

	ret = execl("prova_execl" , "Nome_programma", "uno" , "due" , "tre" , "quattro" , NULL);
	
	
	

	return 0;
}
