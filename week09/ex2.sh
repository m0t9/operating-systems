#!/bin/bash

gcc -Wall -Wextra pager.c -o pager
gcc -Wall -Wextra mmu.c -o mmu

./pager 4 2 $1 &

pid_pager=$!
sleep 1
./mmu 4 W1 W2 R1 W1 R2 R3 W2 W3 R1 W1 R2 W1 W2 R3 R1 R1 W0 R0 W1 W2 $pid_pager
rm pager
rm mmu
