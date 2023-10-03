#!/bin/bash
gcc ~/week04/ex3.c -o ~/week04/ex3
~/week04/ex3 3 &
pstree | grep ex3 
sleep 5
pstree | grep ex3 
sleep 5
pstree | grep ex3 
sleep 5
~/week04/ex3 5 &
pstree | grep ex3 
sleep 5
pstree | grep ex3 
sleep 5
pstree | grep ex3 
sleep 5
pstree | grep ex3
sleep 5
pstree | grep ex3
sleep 5

