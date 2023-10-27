#!/bin/bash

gcc -Wall -Wextra pager.c -o pager
gcc -Wall -Wextra mmu.c -o mmu

./pager 4 2 &
pid_pager=$!
sleep 1
./mmu 4 R0 R1 W1 R0 R2 W2 R0 R3 W2 $pid_pager

rm pager
rm mmu
