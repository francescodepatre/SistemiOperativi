#include <stdio.h>
int main(int argc, char **argv) {
int i;
printf("This programm is %s and his number of arguments is %d.\n" , argv[0], argc);
for (i=1; i< argc; i++){
	printf("The argument number %d is %s.\n",i,argv[i]);
}
return 0;
}

