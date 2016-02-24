#!/bin/sh

echo ===
echo pkgin bash
pkg_add -v bash

echo ===
echo pkgin gmake
pkg_add -v gmake

echo ===
echo pkgin pkg-config
pkg_add -v pkg-config

echo ===
echo pkgin autoconf
pkg_add -v autoconf

echo ===
echo pkgin xfce4
pkg_add -v xfce4

echo ===
echo pkgin gtk2+
pkg_add -v gtk2+

echo ===
echo pkgin git
pkg_add -v git

# mount ?

echo ===
echo mkdir /mnt/hdd1
mkdir -p /mnt/hdd1
return 1

echo ===
echo copy /mnt/work
tar xvpzf /mnt/hdd1/work.tgz -C /mnt

echo ===
echo copy /home/execunix
tar xvpzf /mnt/hdd1/execunix.tgz -C /home
