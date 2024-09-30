#include <unistd.h>
#include <string.h>

int main()
{  
  char s[100];
  size_t sl;   

  strcpy(s, "This data will go to standard output\n"); 
  sl = strlen(s);

  if ((write(1, s, sl)) == -1)
    perror("write error"); /* stampa la stringa e un messaggio di errore */

  return 0;
}
