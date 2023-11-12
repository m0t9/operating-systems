#!/bin/bash
rm -rf ./tmp
ln -s ~/week10/tmp tmp1
ls -li
#total 48
#577779 -rw-rw-r-- 1 m0t9 m0t9 8882 Nov  9 20:38 ex1.c
#566704 -rw-rw-r-- 1 m0t9 m0t9  234 Nov  9 20:36 ex1.sh
#577949 -rw-rw-r-- 1 m0t9 m0t9  849 Nov 12 08:54 ex1.txt
#577338 -rw-rw-r-- 1 m0t9 m0t9  717 Nov 11 20:18 ex21.sh
#577857 -rw-rw-r-- 1 m0t9 m0t9 1475 Nov 12 08:29 ex22.sh
#575893 -rw-rw-r-- 1 m0t9 m0t9  711 Nov 12 08:59 ex23.sh
#577855 -rw-rw-r-- 1 m0t9 m0t9 1437 Nov 12 09:23 ex24.sh
#577936 -rw-rw-r-- 1 m0t9 m0t9   51 Nov 12 09:21 gen.sh
#577220 -rw-rw-r-- 1 m0t9 m0t9 4889 Nov  9 07:18 monitor.c
#577780 lrwxrwxrwx 1 m0t9 m0t9   21 Nov 12 09:23 tmp1 -> /home/m0t9/week10/tmp
mkdir ./tmp
ls -li
#total 52
#577779 -rw-rw-r-- 1 m0t9 m0t9 8882 Nov  9 20:38 ex1.c
#566704 -rw-rw-r-- 1 m0t9 m0t9  234 Nov  9 20:36 ex1.sh
#577949 -rw-rw-r-- 1 m0t9 m0t9  849 Nov 12 08:54 ex1.txt
#577338 -rw-rw-r-- 1 m0t9 m0t9  717 Nov 11 20:18 ex21.sh
#577857 -rw-rw-r-- 1 m0t9 m0t9 1475 Nov 12 08:29 ex22.sh
#575893 -rw-rw-r-- 1 m0t9 m0t9  711 Nov 12 08:59 ex23.sh
#577855 -rw-rw-r-- 1 m0t9 m0t9 1437 Nov 12 09:23 ex24.sh
#577936 -rw-rw-r-- 1 m0t9 m0t9   51 Nov 12 09:21 gen.sh
#577220 -rw-rw-r-- 1 m0t9 m0t9 4889 Nov  9 07:18 monitor.c
#577933 drwxrwxr-x 2 m0t9 m0t9 4096 Nov 12 09:23 tmp
#577780 lrwxrwxrwx 1 m0t9 m0t9   21 Nov 12 09:23 tmp1 -> /home/m0t9/week10/tmp

# Soft link can be created befor creation of the file itself

bash gen.sh 10 ex1.txt
mv ex1.txt ./tmp/ex1.txt

ln -s ~/week10/tmp ~/week10/tmp1/tmp2
bash gen.sh 10 ex1.txt
mv ex1.txt ./tmp1/tmp2

# We can recursively access the subfolders
rm -rf ./tmp
cd tmp1
#bash: cd: tmp1: No such file or directory

rm tmp1
