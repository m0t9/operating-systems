#!/bin/bash
stat ex1

#File: ex1
#Size: 0         	Blocks: 0          IO Block: 4096   regular empty file
#Device: fd00h/64768d	Inode: 577245      Links: 1
#Access: (0664/-rw-rw-r--)  Uid: ( 1000/    m0t9)   Gid: ( 1000/    m0t9)
#Access: 2023-11-12 08:13:23.319770885 +0000
#Modify: 2023-11-12 08:13:23.319770885 +0000
#Change: 2023-11-12 08:13:23.319770885 +0000
#Birth: 2023-11-12 08:13:23.319770885 +0000

cp ex1 ex2
stat ex2

#File: ex2
#Size: 0         	Blocks: 0          IO Block: 4096   regular empty file
#Device: fd00h/64768d	Inode: 577554      Links: 1
#Access: (0664/-rw-rw-r--)  Uid: ( 1000/    m0t9)   Gid: ( 1000/    m0t9)
#Access: 2023-11-12 08:14:49.646267119 +0000
#Modify: 2023-11-12 08:16:06.803435692 +0000
#Change: 2023-11-12 08:16:06.803435692 +0000
#Birth: 2023-11-12 08:14:49.646267119 +0000

# Inodes are different because the content that is stored in memory address
# for the ex1 file was copied to the new area for the file ex2 

ls -l ex1
#-rw-rw-r-- 1 m0t9 m0t9 0 Nov 12 08:13 ex1


stat -c "%h - %n" /etc/* | grep ^3
#3 - /etc/alsa
#3 - /etc/apache2
#3 - /etc/apparmor
#3 - /etc/apport
#3 - /etc/avahi
#3 - /etc/ca-certificates
#3 - /etc/chromium
#3 - /etc/default
#3 - /etc/emacs
#3 - /etc/fwupd
#3 - /etc/gdb
#3 - /etc/ghostscript
#3 - /etc/glvnd
#3 - /etc/gss
#3 - /etc/ifplugd
#3 - /etc/libblockdev
#3 - /etc/libreoffice
#3 - /etc/lvm
#3 - /etc/opt
#3 - /etc/perl
#3 - /etc/pm
#3 - /etc/sane.d
#3 - /etc/sgml
#3 - /etc/ufw
#3 - /etc/update-manager
#
