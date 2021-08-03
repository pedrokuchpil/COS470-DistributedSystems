#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lib.h"

char *checkPrime(char* numero)
{
    int i=0;        
    int n = atoi(numero); //converte de string pra int
    if (n == 1)
    {
        return "Número enviado não é primo\n";
    }
    for (i=2;i<=n/2;i++)
    {
        if(n%i==0)
        {
            return "Número enviado não é primo\n";
        }
    }
    return "Número enviado é primo\n";
}

int generateNumber(int n)
{
    int r = rand()%100 + 1;
    int result = n+r;
    return n+r;
}

char * int_to_str(int num, char *str)
{
    if(str == NULL)
    {
            return NULL;
    }
    sprintf(str, "%d", num);
    return str;
}