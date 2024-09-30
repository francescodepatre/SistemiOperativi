#include <stdio.h>
int main(int argc, char **argv, char **envp)
{
    char *p;
    while (p = *envp)
    {
        printf("%s.\n", p);
        envp += 1;
    }
    return 0;
}