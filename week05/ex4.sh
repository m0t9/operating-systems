#!/bin/bash
declare -i n=10000000
gcc ~/week05/ex4.c -o ~/week05/ex4

printf "Experiment results are:\n" > ex4_res.txt
for i in 1 2 4 10 100;
do
    printf "Time for n = $n, m = $i is\n" >> ex4_res.txt
    /bin/time -p -a -o ex4_res.txt ~/week05/ex4 $n $i 
done

rm ~/week05/ex4
