#include <unistd.h>
#include <stdio.h>


#define N 256

int main()
{
   char buffer[N];
   int nread;

   nread = read(0, buffer, N);
   if (nread == -1)
     perror("read error");
      
   if ((write(1, buffer, nread)) != nread)
     perror("write error");

   return 0;
}
