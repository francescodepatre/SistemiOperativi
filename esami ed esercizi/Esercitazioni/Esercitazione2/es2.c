#include<stdio.h>
#include<unistd.h>
#include<ctype.h>
#define N 256
#include<string.h>
int main() {
	char buffer[N];
	int nread;
	nread = read(0,buffer,N);
	for(int i=0;i<N;i++){
		if(isupper(buffer[i])){
			printf("Carattere scelto risulta:  %c\n" ,buffer[i]);
			char a = buffer[i];
			buffer[i] = a+32;
			}
		}

	if(nread ==-1){
		perror("Read error");
	}

	if((write(1,buffer,nread))!=nread){
		perror("Write error");
	}
return 0;
}


