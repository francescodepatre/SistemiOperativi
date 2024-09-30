#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

pid_t pidpadre;
int tabpid[2];



  struct msg {
    int mypid;
    int n;
  } m1, m2, m3,m4;
  

  
void handler(int signo){
printf("E' stato ricevuto un segnale, sono il processo %d\n", getpid());
exit(0);
}


void figlio(int i, int pfd, int sum,int pf)
{

  
  for(;;){
  read(pfd, &m2, sizeof(m2));
  sum+=m2.n;
  write(pf,&sum,sizeof(sum));
  printf("Sono pid %d Somma = %d\n",getpid(), sum);
  if(sum>20){
  kill(m2.mypid,SIGUSR1);
  break;}
}
printf("La somma finale pid %d è %d e %d\n", getpid(), sum,m2.mypid);


}



int main()
{

	
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



  int pp[2][2];  // Un vettore di vettori (matrice) di file descriptor

  pidpadre= getpid();
int pdpadre[2];
if((pipe(pdpadre))<0){
	perror("Pp error");
};



  //Creo le 2 pipe 

  if(pipe(pp[0])<0) 
    {
      perror("creazione pipe prima pipe:");
      exit(-1);
    }

  if(pipe(pp[1])<0) 
    {
      perror("creazione pipe seconda pipe:");
      exit(-1);
    }


int somma=0;
  //Creo i figli
  for(int i=0;i<2;i++)
    {
      if((tabpid[i]=fork())<0) 
	{
	  perror("fork:");
	  exit(-1);
	}
      if(tabpid[i]==0)
	{
	  figlio(i,pp[i][0],somma,pdpadre[1]);
	  exit(0) ;
	}
    }
    
    int i=0;
    for(;;){
    m1.n =10;
    m1.mypid=getpid();
    printf("pid %d\n",getpid());
    int j=i%2;
    sleep(2);
    write(pp[j][1],&m1,sizeof(m1));
    i++;
    int somme;
    read(pdpadre[0],&somme,sizeof(somme));
    if(somme>20){
    	printf("SOmma vale %d\n", somme);
    	sigsuspend(&zeromask);
    }
   
}


}

