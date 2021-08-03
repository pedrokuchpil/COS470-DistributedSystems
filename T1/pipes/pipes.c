#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>


#define BUFFER 20


void checkPrime(char* numero) //função de checar se o número é primo
{
	int i=0;    
	int n = atoi(numero);
	for (i=2;i<=n/2;i++)
	{
		if(n%i==0)
		{
			printf("Numero nao e primo: '%s'\n",numero);
			return;
		}
	}
	printf ("Numero e primo\n");
	return;
}

int main()
{

    srand(time(NULL));	

    int fd[2];
    pid_t pid;
    int iterations;
    printf("Insira o número de iterações: \n");
    scanf("%d", &iterations);


    if (pipe(fd) < 0 ){
        printf("Erro no Pipe");
        return 1;
    }
    
    if ((pid = fork()) < 0) //criando filho, erro no fork
    {
        printf("ERRO NO FORK\n");
        exit(1);
    }

    if (pid > 0) //fork ocorreu, processo pai (produtor):
    { 
        close(fd[0]);
        int n0 = 1;
		char n1[20];

        while (iterations>0)
        {
            int random = (rand() % 100 + 1);
            n0 += random;
            sprintf(n1, "%i", n0);
            write(fd[1], n1, 21);
            iterations--;
        }
		printf("Encerramento enviado pelo pai no pipe\n");
        write(fd[1], "0", 21);
        exit(0);

    }
    else //processo filho (consumidor)
    {
        char str_recebida[BUFFER];
        close(fd[1]);
        while (true)
        {
            read(fd[0], str_recebida, 21);
            int n5 = atoi(str_recebida);
            if ((n5) == 0)
            {
                break;
            }
            else
            {
            printf("String lida pelo filho no Pipe : '%s' \n", str_recebida);
            checkPrime(str_recebida);
            }
        }
        exit(0);        
    }
	return 0;
}
