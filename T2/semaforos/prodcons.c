#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/time.h>
#include <time.h>

#define M 10000

int *buffer;
sem_t empty;
sem_t full;
sem_t threads_producer, threads_consumer;
pthread_mutex_t mutex;
int n_consumed, n_produced = 0;
int np, nc, n, m;


char *checkPrime(int number)
{
    int i=0;        
    if (number == 1)
    {
        return "Número enviado não é primo\n";
    }
    for (i=2;i<=number/2;i++)
    {
        if(number%i==0)
        {
            return "Número enviado não é primo\n";
        }
    }
    return "Número enviado é primo\n";
}


// função que inicializa um vetor cheio de zeros
int *generateVector(int size)
{
    int *vector = (int *)malloc((size + 1) * sizeof(int));
    for (int i = 0; i < size; i++)
    {
        vector[i] = 0;
    }
    return vector;
}

//gera número aleatório
int generateNumber(int range)
{
    int r = (rand()%range + 1) ;
    return r;
}

//função que retorna uma posição ocupada por 0 em um vetor
int findZeroPosition(int *vector)
{
    int position = -1;//retorna -1 em caso de erro
    for (int i = 0; i < n; i++)
    {
        if (vector[i] == 0)
        {
            position = i;
            break;
        }
    }
    return position;
}

//função que retorna uma posição ocupada por um número não nulo em um vetor
int findFullPosition(int *vector)
{
    int position = -1;//retorna -1 em caso de erro
    for (int i = 0; i < n; i++)
    {
        if (vector[i] != 0)
        {
            position = i;
            break;
        }
    }
    return position;
}

//código executado pela thread produtora
void *producer(void *pno)
{
    while (n_produced < M)
    {
        sem_wait(&empty); //se empty estiver em 0 (não há posições vazias), aguardar
        pthread_mutex_lock(&mutex); //faz o lock do mutex para adentrar a região crítica
        int in = findZeroPosition(buffer); //encontra uma posição com 0 no buffer
        buffer[in] = generateNumber(10000000); //gera um número aleatório e o insere nessa posição
        n_produced++;
        //printf("Producer %d: Insere item %d na posição %d\n", *((int *)pno), buffer[in], in);
        //printf("n_produced: %d\n", n_produced);
        pthread_mutex_unlock(&mutex); //retira lock
        sem_post(&full); //post em full (uma posição da memória foi ocupada)
    }
    sem_post(&threads_producer); //ao produzir M números, post em threads_producer para que todas as threads
                                 //produtoras possam ser canceladas.

}

void *consumer(void *cno)
{
    while (n_consumed < M)
    {
        sem_wait(&full);//se full estiver em 0 (não há posições ocupadas), aguardar
        pthread_mutex_lock(&mutex); //faz o lock do mutex para adentrar a região crítica
        int out = findFullPosition(buffer); //encontra uma posição ocupada no buffer
        int item = buffer[out]; //
        //printf("Consumer %d: Remove item %d da posição %d\n",*((int *)cno), item, out);
        //printf("%s", checkPrime(item)); //roda a função checkPrime
        n_consumed++;
        //printf("n_consumed: %d\n", n_consumed);
        buffer[out] = 0; //seta o valor da posição consumida em zero
        pthread_mutex_unlock(&mutex); //retira lock
        sem_post(&empty); //adiciona um valor em empty
    }
    sem_post(&threads_consumer); //ao consumir M números, post em threads_consumer para que todas as threads
                                 //consumidoras possam ser canceladas.
}

int main (int argc, char *argv[]) 
{
    srand(time(NULL)); //inicialização da chave para geração de números aleatórios
    struct timeval t1, t2;
    double elapsedTime;

    np = atoi(argv[1]);
    nc = atoi(argv[2]);
    n = atoi(argv[3]);

    //printf("%d %d %d \n", np, nc, n);

    pthread_t producers[np];
    pthread_t consumers[nc];
    int producersId[np];
    int consumersId[nc];

    sem_init(&empty,0,n); //inicia o semaforo empty com valor n (tamanho do vetor)
    sem_init(&full,0,0); //inicia o semaforo full com valor 0

    //inicia os semaforos de threads com 0, para que o post realizado no código
    //das threads permita o cancelamento das mesmas
    sem_init(&threads_producer,0,0);
    sem_init(&threads_consumer,0,0);


    buffer = generateVector(n); //inicalização da memória

    gettimeofday(&t1, NULL); //tempo de início

    //cria threads produtoras e consumidoras
    for(int i = 0; i < np; i++) 
    {
        producersId[i] = i;
        pthread_create(&producers[i], NULL, (void *)producer, (void *)&producersId[i]);
    }

    for(int i = 0; i < nc; i++) 
    {
        consumersId[i] = i;
        pthread_create(&consumers[i], NULL, (void *)consumer, (void *)&consumersId[i]);
    }

    sem_wait(&threads_producer); //espera para poder cancelar threads produtoras
    for(int i = 0; i < np; i++) 
    {
        pthread_cancel(producers[i]);
    }
    
    sem_wait(&threads_consumer); //espera para poder cancelar threads consumidoras
    for(int i = 0; i < nc; i++) 
    {
        pthread_cancel(consumers[i]);
    }

    gettimeofday(&t2, NULL); //tempo final

    elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0;      // sec to ms
    elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0;   // us to ms
    printf("%.0f\n", elapsedTime);
    
    //encerra mutex e semaforos
    pthread_mutex_destroy(&mutex);
    sem_destroy(&empty);
    sem_destroy(&full);
    sem_destroy(&threads_producer);
    sem_destroy(&threads_consumer);

    free(buffer);

    return 0;
}