#!/bin/bash

gcc -Wall -Wextra -Werror ex2.c -o ex2
./ex2 $1
rm ex2
