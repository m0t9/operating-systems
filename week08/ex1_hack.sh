#!/bin/bash

pid=$(cat /tmp/ex1.pid)

heap=$(grep "heap" /proc/$pid/maps) 
range=$(echo $heap | cut -d ' ' -f 1)
begin=0x$(echo $range | cut -d '-' -f 1)
end=0x$(echo $range | cut -d '-' -f 2)

password_line=$(sudo xxd -s $begin -l $(($end - $begin)) /proc/$pid/mem | grep "pass:")
address=0x$(echo $password_line | cut -d ':' -f 1)
password_unrestricted=$(echo $password_line | grep -o "\bpass:.*") 
password=$(echo $password_unrestricted | cut -c 1-13)

echo "Memory address of password is $address" 
echo $password

kill -SIGKILL $pid
