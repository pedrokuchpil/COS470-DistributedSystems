//Desenvolvido a partir do tutorial disponível em https://www.linuxhowtos.org/C_C++/socket.htm

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

#define BUFFER_SIZE 256

char *checkPrime(char* número)
{
	int i=0;    
	int n = atoi(número); //converte de string pra int
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

int main(int argc, char *argv[])
{
   int sockfd, newsockfd, n_port;
   socklen_t clilen;
   char buffer[BUFFER_SIZE];
   struct sockaddr_in serv_addr, cli_addr;
   int n;

   if (argc < 2) 
   {
      printf("Forneça a porta\n");
      return 1;
   }

   sockfd = socket(AF_INET, SOCK_STREAM, 0); //chamada de sistema que cria um novo socket, retornando um descritor de arquivo para o mesmo 
                                             //AF_INET é padrão para hosts na Internet, e SOCK_STREAM, 0 gera o tipo de socket para o protocolo 
                                             //TCP

   if (sockfd < 0)
   {
      printf("Erro abrindo socket");
      return 1;
   }

   n_port = atoi(argv[1]); //lê número da porta e converte para int

   bzero((char *) &serv_addr, sizeof(serv_addr)); //inicializa o endereço do servidor como 0
   serv_addr.sin_family = AF_INET; //familia de endereços
   serv_addr.sin_addr.s_addr = INADDR_ANY; //endereço da máquina que está rodando o código
   serv_addr.sin_port = htons(n_port); //altera o endianness (necessário para rede) do número da porta

   if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) //chamada de sistema que liga um socket a um endereço
   {
      printf("Erro na ligação");
      return 1;
   }

   listen(sockfd,1); //chamada de sistema para o processo ouvir o socket por novas conexões

   clilen = sizeof(cli_addr);
   newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen); //chamada accept bloqueia o processo até que uma conexão com um cliente seja estabelecida,
                                                                     //retornando um novo descritor de arquivo
   if (newsockfd < 0)
   {
      printf("Erro na chamada accept");
      return 1;
   }

   while (1) //enquanto não ler número 0
   {
      bzero(buffer,BUFFER_SIZE); //zera o buffer
      n = read(newsockfd,buffer,BUFFER_SIZE); //lê string do socket
      if (n < 0)
      {
         printf("Erro lendo do socket");
         return 1;
      }
      printf("Numero recebido: %s\n",buffer);
      if (atoi(buffer) == 0)
      {
         break;
      }
      n = write(newsockfd,checkPrime(buffer),BUFFER_SIZE); //escreve resultado de checkPrime da string recebida no socket
      if (n < 0)
      {
         printf("Erro escrevendo no socket");
         return 1;
      } 
   }

   //fecha os descritores de arquivo do socket
   close(newsockfd);
   close(sockfd);
   return 0; 
}