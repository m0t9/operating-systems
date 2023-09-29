#!/bin/bash
gcc ~/week04/ex3.c -o ~/week04/ex3
~/week04/ex3 3 &
pstree 
sleep 5
pstree 
sleep 5
pstree 
sleep 5
~/week04/ex3 5 &
pstree 
sleep 5
pstree 
sleep 5
pstree 
sleep 5
pstree
sleep 5
pstree
sleep 5

