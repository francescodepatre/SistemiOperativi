#include <unistd.h>

int main()
{
   char buffer[128];
   int nread;

   nread = read(0, buffer, 128);
   if (nread == -1)
     perror("read error");
      
   if ((write(1, buffer, nread)) != nread)
     perror("write error");

   return 0;
}
