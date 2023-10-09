#!/bin/bash
gcc -Wall ~/week05/subscriber.c -o ~/week05/sub
gcc -Wall ~/week05/publisher.c -o ~/week05/pub

n=$1

gnome-terminal -- ~/week05/pub $n 

for ((i = 1; i <= $n; i++))
do
    gnome-terminal -- ./sub $i
done
 
rm ~/week05/pub
rm ~/week05/sub

