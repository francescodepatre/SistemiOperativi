#include<stdio.h>
#include<stdlib.h>  /* serve per rand() e srand() */
#include<time.h> 
#include<unistd.h>
#include<signal.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<fcntl.h>
#include<unistd.h>



void handler(int signo){
printf("E' stato ricevuto un segnale, sono il processo %d\n", getpid());
}

int main(int argc, char **argv){
if(argc != 3){
	printf("Devi inserire due valori\n");
}

  struct msg {
    int mypid;
    int n;
  } m1, m2, m3,m4;

int status;
int N = atoi(argv[1]);
pid_t pid,pid2;

//preparazione segnale SIGUSR1
sigset_t set,zeromask;
struct sigaction action;
sigemptyset(&zeromask); 
	
sigemptyset(&action.sa_mask);
action.sa_handler = handler;
action.sa_flags =0;
sigemptyset(&set); 
sigaddset(&set, SIGUSR1); 
sigprocmask(SIG_BLOCK, &set , NULL); 

if(sigaction(SIGUSR1, &action, NULL)==-1){
perror("Sigaction error");}


int pdpadre[2], pdfiglio1[2], pdfiglio2[2];
//GENERAZIONE DI DUE PROCESSI FIGLI E PIPE

if(pipe(pdfiglio2)<0){
	perror("pip error");
}

if(pipe(pdfiglio1)<0){
	perror("pip error");
}

if(pipe(pdpadre)<0){
	perror("pip error");
}

if((pid = fork())<0){
perror("fork error");
}

if(pid==0){
//primo figlio

m1.mypid=getpid();
srand(getpid());
int random2;
int random = (int) ((N+1) * (rand()/(RAND_MAX+1.0)));
m1.n=random;
printf("Il mio pid è %d , sono figlio di %d e il numero random comperso tra 1 e %d è %d\n" , m1.mypid, getppid(), N, random);


//Scriviamo sulla pipe del figlio2 il pid del figlio1
write(pdfiglio2[1], &m1, sizeof(m1));
read(pdfiglio1[0], &m3, sizeof(m3));

printf("Il pid di mio fratello è %d e il suo numero random è %d\n", m3.mypid, m3.n);

//Confrontiamo random figlio1 con random figlio2 se minore lancio segnale, se maggiore attendo
if(m1.n<m3.n){
kill(m3.mypid, SIGUSR1);
printf("Sono quello che ha inviato il segnale ed il mio pid vale %d\n", m1.mypid);
}
else{
sigsuspend(&zeromask);
printf("Ho ricevuto il segnale, ora termino\n");
exit(m3.mypid);
}

}

else if(pid>0) {

//padre

if((pid2=fork())<0){
perror("Fork error");
}

if(pid2==0){
//Secondo figlio

m2.mypid = getpid();
int random2;
srand(getpid());
int random = (int) ((N+1) * (rand()/(RAND_MAX+1.0)));
m2.n = random;
printf("CIao, Il mio pid è %d , sono figlio di %d e il numero random comperso tra 1 e %d è %d\n" , m2.mypid, getppid(), N, random);


//Scriviamo sulla pipe del figlio2 il pid del figlio1
write(pdfiglio1[1], &m2, sizeof(m2));
read(pdfiglio2[0], &m4, sizeof(m4));

printf("Il pid di mio fratello è %d e il suo numero random è %d\n", m4.mypid, m4.n);

//Confrontiamo random figlio1 con random figlio2 se minore lancio segnale, se maggiore attendo
if(m2.n<m4.n){
kill(m4.mypid, SIGUSR1);
printf("Sono quello che ha inviato il segnale ed il mio pid vale %d\n", m2.mypid);
}
else{
sigsuspend(&zeromask);
printf("Ho ricevuto il segnale, ora termino\n");
exit(m4.mypid);
}

}

else{
//padre

printf("stato %d\n",wait(NULL));
if((waitpid(pid, &status,0))>0){	
		if(WIFEXITED(status)){
			printf("exit del processo con %d\n", getpid(), WEXITSTATUS(status));
		}
		}
exit(0);
}


}



return 0;
}
