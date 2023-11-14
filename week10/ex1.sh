#!/bin/bash

gcc -Wextra -Wall ex1.c -o ex1 
gcc -Wextra -Wall monitor.c -o monitor

gnome-terminal -- ./monitor $1
pid_monitor=$!

sleep 1

./ex1 $1

rm ex1
rm monitor


