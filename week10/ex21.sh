#!/bin/bash

ls -l /etc | grep ^d | wc -l
# 132
ls -l /dev | grep ^d | wc -l
# 18

# English Hello World
file ex1.c
# ex1.c: C source, ASCII text
gcc ex1.c -o ex1
file ex1
# ex1: ELF 64-bit LSB pie executable, ARM aarch64, version 1 (SYSV), dynamically linked, interpreter /lib/ld-linux-aarch64.so.1, BuildID[sha1]=99e20712f35bf708a6d95d2e9f7c2c5c49af69fe, for GNU/Linux 3.7.0, not stripped

# Russian Hello World
file ex21.c
# ex21.c: C source, Unicode text, UTF-8 text
gcc ex1.c -o ex1
file ex1
# ex1: ELF 64-bit LSB pie executable, ARM aarch64, version 1 (SYSV), dynamically linked, interpreter /lib/ld-linux-aarch64.so.1, BuildID[sha1]=9e504427bf2d8533a05c055ecb79e725f7493f04, for GNU/Linux 3.7.0, not stripped

