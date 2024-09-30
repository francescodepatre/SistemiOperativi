#include <unistd.h>
#include<stdio.h>
#include<fcntl.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include <string.h>
#define N 3
#define M 28
#define PERM 0644
//dichiarazione struct
typedef struct persona 
{
	char nome[11];
	char eta[4];
} Persona;


int main(int argc) {
Persona vettore[N];
strcpy(vettore[0].nome, "Giovanni ");
strcpy(vettore[1].nome ,"Marcolino ");
strcpy(vettore[2].nome, "Petrellio ");
strcpy(vettore[0].eta, "12\n");
strcpy(vettore[1].eta, "21\n");
strcpy(vettore[2].eta, "34\n");


printf("%d" , argc);

int outfile,outfile2;
char buffer [M];
outfile = open("vett.txt", O_CREAT|O_RDWR | O_TRUNC, PERM);
outfile2 = open("vett2.txt", O_CREAT|O_WRONLY | O_TRUNC, PERM);
int i=0; 
while(i <N){
write(outfile, vettore[i].nome ,10 );
write(outfile, vettore[i].eta ,4);
i++;
}

long int cur_pos;
cur_pos = lseek(outfile,0,SEEK_SET);
cur_pos = lseek(outfile, argc*28,SEEK_CUR);
read(outfile, buffer, 28);
write(outfile2,buffer,14);
for(int i=0;i<N;i++){
	printf("nome: %s età: %s\n", vettore[i].nome, vettore[i].eta);
}

close(outfile);

return 0;

}

