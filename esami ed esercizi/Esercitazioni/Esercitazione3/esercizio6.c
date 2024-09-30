#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>
#include<stdio.h>
#include<sys/types.h>
#include<string.h>
#include<time.h>

int main(int argc, char **argv){
int pid;
long int status;
//CONTROLLO ARGOMENTI
	if(argc<2){
		printf("Attento! Devi scrivere ./nome arg1 arg2\n");
		exit(-1);
		}
//STAMPA ARGOMENTI
	printf("Gli argomenti ricevuti sono:\n");
	for(int i=0;i<argc;i++){
		printf("%s\n", argv[i]);
	}

//VERIFICA CHE ARGV SIA MINORE DI 10, IN CASO CONTRARIO TERMINA
	int argomento = atoi(argv[1]);
	if(argomento >=10){
		printf("E' stato inserito un argomento maggiore o uguale a 10, ora termino\n");
		exit((atoi(argv[2])));	
	}

//PROCEDO ALLA CREAZIONE DEL FIGLIO	
	pid = fork();
	if(pid <0){
		perror("Fork() error");
		exit(-1);
	}
	
//ORA FACCIO ESEGUIRE AL FIGLIO IL PROGRAMMA i-esimo
	if(pid ==0){

//PRENDIAMO IL PRIMO ARGOMENTO, LO AUMENTIAMO DI 1 E LO SALVIAMO NELA STRINGA CONC DOPO AVERLO CONVERTITO. DOPODICHE' FACCIAMO IL %3 SUL NUMERO OTTENUTO E SALVIAMO IL RISULTATO, DOPO AVERLO CONVERTITO, NELLA STRINGA CONC1.
//CONCATENIAMO LA STRINGA "Progetto", CHE È L'INIZIO DI OGNI NOME DEI PROGRAMMI, ALLA STRINGA CONC PER SPECIFICARE QUALE PROGRAMMA ANDRÀ ESEGUITO.
//MODIFICHIAMO IL SECONDO ARGOMENTO AGGIUNGENDO UN NUMERO RANDOM TRA 0 E 10, FACENDO SEMPRE LE CONVERSIONI DA STRINGA A INTERO E SUCCESSIVAMENTE DA INTERO A STRINGA
//INFINE METTIAMO NEL VETTORE ARGS IL NOME DEL PROGRAMMA DA ESEGUIRE, ARGV[1]+1 E ARGV[2]+RANDOM E NULL CHE SARANO GLI ARGOMENTI PER L'EXECV.

		int to_arg = atoi(argv[1]);	
		to_arg +=1;
		int to_arg3 = to_arg%3;
		char conc[4];
		sprintf(conc, "%d", to_arg3);
		char conc1[4];
		sprintf(conc1, "%d", to_arg);		
		char prog[9] = "Progetto";
		strcat(prog, conc);
		int to_arg2 = atoi(argv[2]);
		srand(getpid());
		to_arg2 = to_arg2 + rand()%10;
		char conc2[4];
		sprintf(conc2, "%d", to_arg2);
		char *args[] = {prog, conc1, conc2, NULL};
		
		printf("Sono il figlio con pid %d, ora verrà eseguito il codice Progetto%s\n", getpid(), conc);
		execv(prog,  args);

	}

	else if(pid>0){
		if((waitpid(pid, &status,0))>0){	
		if(WIFEXITED(status)){
			printf("exit del processo con pid %d con %ld\n", getpid(), WEXITSTATUS(status));
		}
		}exit(WEXITSTATUS(status)+(atoi(argv[2])));
	}







}
