#!/bin/bash
declare -i n=10000000
gcc ~/week05/ex3.c -o ~/week05/ex3

printf "Experiment results are:\n" > ex3_res.txt
for i in 1 2 4 10 100;
do
    printf "Time for n = $n, m = $i is\n" >> ex3_res.txt
    /bin/time -p -a -o ex3_res.txt ~/week05/ex3 $n $i 
done

rm ~/week05/ex3
