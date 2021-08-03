#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <unistd.h>

bool term = false;

void signal_handler (int sigNumber) //função que recebe um sinal, imprime seu número na tela e trata alguns deles
{
    printf("Sinal de número %d recebido\n", sigNumber);
    if (sigNumber == SIGTERM)
    {	
        printf("Encerrando...\n");
        term = true;
    }
    else if (sigNumber == SIGINT)
    {
        printf("Esse processo é encerrado com SIGTERM, não com SIGINT\n");
    }
    else if (sigNumber == SIGQUIT)
    {
        printf("Esse processo é encerrado com SIGTERM, não com SIGQUIT\n");
    }
}

int main()
{

    int typewait;
    printf("Busy ou blocking wait (0 ou 1)? ");
	scanf("%d", &typewait);

    //definição de quais sinais serão tratados pela função signal_handler
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    signal(SIGQUIT, signal_handler); 

    while(!term) //enquanto term não for true (comando dado após recebimento do sinal SIGTERM)
    {
        printf("Processo número %d\n", getpid()); //imprime PID do processo
        if (typewait == 1) //se for blocking wait
        {
            pause(); //bloqueia até a chegada de um sinal
        }
        //se for busy wait, fica em loop esperando um sinal
    }
    
    return 0;
}