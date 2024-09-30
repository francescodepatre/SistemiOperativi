#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include <time.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>

#define DEFAULTPORT 9876	/* E' una porta non privilegiata , mentre 987 e' privilegiata e utilizzabile solo da processi di root */

int got_sigusr1=0;

void sig_handler(int signo)
{
  got_sigusr1=1;
  printf("Got SIGUSR1: start logging client accesses\n");
}



main(int argc, char *argv[])
{
  int	sock, length ;
  struct 	sockaddr_in 	server,client;
  struct hostent	*hp,*gethostbyname();
  struct sigaction act;
  int	msgsock, rval,sval1,sval2, i, pid;
  int piped[2];
  time_t now;
  char message[1024],*p;
  char cmd1[128];
  char cmd2[128];
  char *args[2];

  char *badrequest_message="Errore: il comando inviato non ha la forma prevista cmd1|cmd2\n";

  FILE *fp=NULL;

  if(argc !=1)
    {
      fprintf(stderr,"Usage: %s\n",argv[0]);
      exit(-1);
    }



  /* Gestione segnali */
  
  act.sa_handler= sig_handler ;
  sigemptyset(&act.sa_mask);
  act.sa_flags = SA_RESTART ;
  
  if(sigaction(SIGUSR1,&act,NULL)<0)
    {
      perror("sigaction: ");
      exit(2);
    }
  


/* Crea la socket STREAM */
sock=	socket(AF_INET,SOCK_STREAM,0);
if(sock<0)
	{
	perror("opening socket: ");
	exit(1);
	}

/* Name socket using wildcards */
server.sin_family = 	AF_INET;
server.sin_addr.s_addr= INADDR_ANY;
server.sin_port = htons(DEFAULTPORT);

if (bind(sock,(struct sockaddr *)&server,sizeof server)<0)
	{
	perror("binding socket: ");
	exit(1);
	}

/* Ottiene e visualizza la porta ottenuta  */
length= sizeof server;
if(getsockname(sock,(struct sockaddr *)&server,&length)<0)
	{
	perror("getsockname: ");
	exit(1);
	}

printf("Server running on port #%d\n",ntohs(server.sin_port));

  /* Pronto ad accettare connessioni */

  listen(sock,4);


  do{
    
    
    /* Attesa di una connessione */	 
    
    msgsock= accept(sock,(struct sockaddr *)&client,(int *)&length);
    
    
    if(got_sigusr1)
      {
	if(fp == NULL)
	  {
	    if((fp=fopen("server-log.txt","w"))==NULL)
	      {
		perror("opening log file: ");
		exit(-1);
	      }
	  }
	now= time(0);
	fprintf(fp,"%s - client %s port %d connected\n",ctime(&now),inet_ntoa(client.sin_addr), ntohs(client.sin_port) );
	fflush(fp);
      }


    if(fork()==0)
      {
	close(sock);

	if(( rval = read(msgsock,message,sizeof message))<0)
	  {
	    perror("reading from socket: ");
	    exit(-1);
	  }

	if(!rval)
	  {
	    fprintf(stderr,"Client %s port %d unexpectedly closed the connection\n",inet_ntoa(client.sin_addr),    ntohs(client.sin_port));
	    exit(-2);
	  }

	// system(message);   troppo facile !!!
	//
	// "ls|wc"  "ls |wc" "ls |    wc"

	p= strchr(message,'|');

	if(p == NULL)
	  {
	    fprintf(stderr,"Bad request (%s) from client %s port %d\n", message,inet_ntoa(client.sin_addr),    ntohs(client.sin_port) );
	    write(msgsock,badrequest_message, strlen(badrequest_message)+1);
	    exit(-3);
	  }

	
	*p='\0';
	sval1= sscanf(message,"%s",cmd1);
	sval2=  sscanf(p+1,"%s",cmd2);

	if(sval1 != 1 || sval2 != 1)
	  {
	    fprintf(stderr,"Bad request from client %s port %d \n", inet_ntoa(client.sin_addr),    ntohs(client.sin_port) );
	    write(msgsock,badrequest_message, strlen(badrequest_message)+1);
	    exit(-3);
	  }
	
	if(pipe(piped)<0)
	  {
	    perror("creating pipe: ");
	    exit(-4);
	  }



	if( (pid=fork())<0)
	  {
	    perror("forking2: ");
	    exit(-5);
	  }



	if(pid==0)
	  {
	    /* Nipote */
	    
	      
	    /* Ridirezione dello stdout sulla pipe  */

	    close(1);		
	    dup(piped[1]);
	    close(piped[0]);
	    close(piped[1]);
	    
	    args[0]=cmd1;
	    args[1]=NULL;
	
	    execvp(cmd1,args);
	    fprintf(stderr,"execvp of %s failed: %s\n",cmd1,strerror(errno));
	    exit(-6);
	  }
	else
	  {

	    /* Ridirezione dello stdin sulla pipe  */
	    close(0);		

	    dup(piped[0]);
	    close(piped[0]);
	    close(piped[1]);
	    

	    /* Ridirezione dello stdout sulla socket connessa  */

	    close(1);
	    dup(msgsock);	
	    
	    close(msgsock);
	    
	    args[0]=cmd2;
	    args[1]=NULL;

	    execvp(cmd2,argv);
	    fprintf(stderr,"execvp of %s failed: %s\n",cmd2,strerror(errno));
	    exit(-7);
	  }

      }
    else
      {close(msgsock);}
  }
  while(1);


}
