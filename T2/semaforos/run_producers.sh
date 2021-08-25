#!/bin/bash

gcc -pthread -o prodcons prodcons.c

for n in 1 2 4 16 32
do
    for i in 1 2 4 8 16
    do
        SOMA=0
        for j in $(seq 1 10);
        do
            OUTPUT=$(./prodcons $i 1 $n)
            let "SOMA=SOMA+OUTPUT"
        done
        echo "$i 1 $n"
        MEDIA=$((SOMA/10))
        echo $MEDIA
    done
done
