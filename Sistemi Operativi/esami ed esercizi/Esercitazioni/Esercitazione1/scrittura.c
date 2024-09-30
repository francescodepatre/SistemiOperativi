#include<unistd.h>
#include<string.h>

int main(){
char s[100];
size_t s1;
strcpy(s, "Questa stringa andrà sullo standard output\n");
s1 = strlen(s);

if((write(1,s,s1))==1){
perror("Write error: ");
}

return 0; }
