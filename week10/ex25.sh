#!/bin/bash
touch ex5.txt
chmod a-w ex5.txt
chmod a+rwx ex5.txt
chmod g= ex5.txt
# 660 = rw-rw---- (user, group - can write and read, others can do nothing)
# 775 = rwxrwxr-x (user, group - can write, read, execute, others can read and execute)
# 777 = rwxrwxrwx (all can write, read, execute)
