#!/bin/bash

gcc -pthread -o prodcons prodcons.c

for n in 1 2 4 16 32
do
    for i in 1 2 4 8 16
    do
        SOMA=0
        for j in $(seq 1 10);
        do
            OUTPUT=$(./prodcons 1 $i $n)
            let "SOMA=SOMA+OUTPUT"
        done
        echo "1 $i $n"
        MEDIA=$((SOMA/10))
        echo $MEDIA
    done
done
