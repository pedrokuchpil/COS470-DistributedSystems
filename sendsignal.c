#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>


int main()
{
	int pid;
	int sinal;
	printf("Entre com o pid: \n");
	scanf("%d", &pid);
	if ((getpgid(pid) < 0)) //checa se o process group ID é inválido (fazer essa verificação diretamente mandando um sinal para o PID pode resultar em erros)
	{
		printf("PID nao encontrado! Tente novamente\n");
		return 1;
	}
	printf("Entre com o número do sinal: \n");
	scanf("%d", &sinal);
	if (sinal > 64) // Número de sinais existentes 
	{
		printf("Sinal não é válido\n");
		return 1;
	}

	kill(pid, sinal); //envia sinal ao processo

	return 0;
}