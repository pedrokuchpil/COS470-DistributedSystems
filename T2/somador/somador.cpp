#include <atomic>
#include <math.h>
#include <unistd.h>
#include <stdlib.h>
#include <iostream>
#include <thread>
#include <chrono>


using namespace std;

class SpinLock
{
private:
    atomic_flag lock = ATOMIC_FLAG_INIT;

public:
    void acquire()
    {
        while (lock.test_and_set())
        {//enquanto a função test_and_set() não conseguir o acquire do lock, ficara em loop
        }
    }
    void release()
    {
        lock.clear();
    }
};

char *vetor;

thread **threads;

SpinLock spinlock;

int result = 0;


int generateNumber()
{
    return (rand() % 200) - 100;
}

void randArray(char *array, int size)
{
    //preenche o array com números aleátorios
    for (int i = 0; i < size; i++)
    {
        array[i] = (char)generateNumber(); //char pra para numero ser 1 byte
    }
}

// função somadora - somador
void somador(char *parse, int size)
{
    /* parse indica o primeiro elemento do vetor dividido a ser somado, enquanto size é seu tamanho*/
    int somalocal = 0;
    for (int i = 0; i < size; i++)
    {
        somalocal += (int)parse[i];
    }
    spinlock.acquire();
    result += somalocal; //somamos a soma parcial ao acumulador global
    spinlock.release();
}


void criaT(int K, char *vetor, int N)           // criação das threads
{
    int parse = N / K; //como indicado no problema, parcelas dividindo o vetor em k threads
    threads = new thread *[K];
    // cria N threads
    for (int i = 0; i < K - 1; i++) //o for vai até a penúltima thread, pois a ultima fica com a parcela + o que sobrou
    {
        threads[i] = new thread(somador, &vetor[i * parse], parse); 
    }
    // ultima thread fora do for 
    int resto = parse + N % K;

    threads[K - 1] = new thread(somador, &vetor[parse * (K - 1)], resto);
}

void alocaT(int K)                //alocação das threads
{
    for (int i = 0; i < K; i++)
    {
        threads[i]->join();
    }
}

int main(int argc, char *argv[])
{
    srand(time(NULL));

    int K; //numero de threads
    int N; //tamanho do array
    K = atoi(argv[1]);
    N = atoi(argv[2]);
    vetor = new char[N];
    randArray(vetor, N);        //gera o vetor tamanho N aleatório

    auto inicio = chrono::high_resolution_clock::now(); //inicia contagem

    criaT(K, vetor, N);
    alocaT(K);       

    auto fim = chrono::high_resolution_clock::now(); //fim da contagem
    auto dur = fim - inicio;
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(dur).count();
    cout << ms << endl;

    return 0;
}