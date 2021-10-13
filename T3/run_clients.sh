#!/bin/bash
g++ -o client client.cpp

mkdir teste0 teste1 teste2 teste3

#TESTE 0

for i in $(seq 1 2);
do
    ./client 10 2 &
done
wait $(jobs -p)

python3 validator.py 10 2 ./console.log ./resultado.txt

cp console.log teste0/
cp resultado.txt teste0/
 
rm console.log
rm resultado.txt

#TESTE 1

for i in 1 2 4 8 16 32;
do
    for j in $(seq 1 $i);
    do
        ./client 10 2 &
    done
    wait $(jobs -p)
    python3 validator.py 10 $i ./console.log ./resultado.txt

    cp console.log teste1/console_$i.log
    cp resultado.txt teste1/resultado_$i.txt

    rm console.log
    rm resultado.txt
done

#TESTE 2

for i in 1 2 4 8 16 32 64;
do
    for j in $(seq 1 $i);
    do
        ./client 5 1 &
    done
    wait $(jobs -p)
    python3 validator.py 5 $i ./console.log ./resultado.txt

    cp console.log teste2/console_$i.log
    cp resultado.txt teste2/resultado_$i.txt

    rm console.log
    rm resultado.txt

done

#TESTE 3

for i in 1 2 4 8 16 32 64 128;
do
    for j in $(seq 1 $i);
    do
        ./client 3 0 &
    done
    wait $(jobs -p)
    python3 validator.py 3 $i ./console.log ./resultado.txt

    cp console.log teste3/console_$i.log
    cp resultado.txt teste3/resultado_$i.txt

    rm console.log
    rm resultado.txt

done