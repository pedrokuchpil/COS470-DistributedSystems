#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>

#define BUFFER_SIZE 256

void error(const char *msg)
{
    perror(msg);
    exit(0);
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

int main(int argc, char *argv[])
{
    int sockfd, n_port, x;
    struct sockaddr_in serv_addr;
    struct hostent *server; //estrutura que define um host
    char buffer[BUFFER_SIZE];

    srand((unsigned int)time(NULL)); //chave para geração de números aleatórios

    //código semelhante ao do servidor    
    if (argc < 3) 
    {
       printf("Uso: %s hostname port\n", argv[0]);
       return 0;
    }
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        printf("Erro abrindo socket");
        return 1;
    } 
    server = gethostbyname(argv[1]); //função que retorna um host por seu nome
    if (server == NULL) 
    {
        printf("Erro, host não existe\n");
        return 0;
    }

    n_port = atoi(argv[2]);

    //adiciona valores aos campos de endereço, assim como no código do servidor
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(n_port);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) //função que faz a conexão com o servidor 
    {
        printf("Erro na conexão");
        return 1;
    } 
    
    int iterations;
    printf("Insira o número de iteracoes: ");
    scanf("%d", &iterations);

    int n0 = 1; //n0
    
    while (iterations > 0)
    {
        int_to_str(n0, buffer); 
        printf("Enviando %s...\n", buffer);
        x = write(sockfd,buffer,strlen(buffer)); //envia número gerado ao servidor
        if (x < 0)
        {
            printf("Erro escrevendo no socket");
            return 1;
        } 
            
        bzero(buffer,BUFFER_SIZE);
        x = read(sockfd,buffer,BUFFER_SIZE); //lê a resposta do servidor
        if (x < 0)
        {
            printf("Erro lendo do socket");
            return 1;
        } 
        printf("%s\n",buffer);

        n0 = generateNumber(n0); //gera novo número
        bzero(buffer,BUFFER_SIZE);

        iterations--;
    }

    bzero(buffer,BUFFER_SIZE);
    int_to_str(0, buffer);
    printf("Enviando %s...\n", buffer);
    x = write(sockfd,buffer,strlen(buffer));
    
    close(sockfd);
    return 0;
}