#!/bin/bash

gcc -Wextra -Wall ex1.c -o ex1 
gcc -Wextra -Wall monitor.c -o monitor


gnome-terminal -- ./monitor .
pid_monitor=$!

sleep 1

./ex1 .

rm ex1
rm monitor
rm -rf myfile11.txt  myfile13.txt  tmp


