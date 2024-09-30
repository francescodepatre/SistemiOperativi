#include <fcntl.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define N 256

int main (int argc, char **argv)
{
    int nread, nwrite = 0, atteso, status, fileh, pid;
    char st1[N];
    char st2[N];
    if (argc != 2)
    {
        fprintf(stderr,"Uso: %s nomefile\n",argv[0]);
        //exit(1);
    }
    /* APERTURA IN LETTURA/SCRITTURA */
    fileh = open(argv[1], O_CREAT|O_RDWR|O_TRUNC, 0644);
    if (fileh == -1) {
        perror("Errore open");
        exit(2);
    }
    if((pid=fork()) < 0)
    {
        perror("Errore fork");
        close(fileh);
        exit(3);
    }
    else if (pid==0)
    {
        /* FIGLIO: legge una stringa che l’utente immette da tastiera */
        printf("Scrivere una stringa (senza spazi) e premere Invio\n");
        scanf("%s",st1);
        nwrite = write(fileh, st1, strlen(st1)+1);
        if (nwrite == -1) {
            perror("Errore write");
            exit(4);
        }
        exit(0);
    }
    else
    {
        atteso=wait(&status); /* ATTESA DEL FIGLIO */
        printf("Il figlio con PID=%d e’ terminato con stato=%d\n",atteso,WEXITSTATUS(status));
        // Riposizionamento del file offset all’inizio del file
        lseek(fileh, 0, SEEK_SET);
        nread = read(fileh, st2, N);
        if (nread == -1)
        {
            perror("Errore read");
        }
        
        printf("nread=%d\n",nread);
        printf("Il figlio ha scritto la stringa %s\n", st2);
        close(fileh);
        return(0);
    }
    exit(0);
}