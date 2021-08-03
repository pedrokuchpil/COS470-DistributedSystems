#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>
#include"../lib/lib.h"


#define BUFFER 20

int main()
{
    srand(time(NULL));	

    int fd[2]; //representação do pipe
    pid_t pid; //pid do processo
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
        close(fd[0]); //fecha o lado do filho no pipe
        int n0 = 1; //n0 inicial (1)
		char n1[20];

        while (iterations>0) //pra toda iteração roda a soma do anterior co
        {
            int_to_str(n0, n1); 
            write(fd[1], n1, 21); //escreve no pipe
            n0 = generateNumber(n0); //gera novo número
            iterations--;
        }
		printf("Encerramento enviado pelo pai no pipe\n");
        write(fd[1], "0", 21); //escreve código 0 após a última iteração
        exit(0);

    }
    else //processo filho (consumidor)
    {
        char str_recebida[BUFFER];
        close(fd[1]); //fecha o lado do pai no pipe
        while (true)
        {
            read(fd[0], str_recebida, 21);
            int msg = atoi(str_recebida);
            if ((msg) == 0) //para com o código 0
            {
                break;
            }
            else
            {
                printf("String lida pelo filho no Pipe : '%s' \n", str_recebida);
                printf("%s\n",checkPrime(str_recebida)); //checa se é primo
            }
        }
        exit(0);        
    }
	return 0;
}

