#!/bin/sh 
. ./setPath.sh
chmod u+x scripts/*.sh
make distclean
make nuc972_rainbow_defconfig
make  -j 32
# file u-boot.bin and u-boot-spl.bin in the spl directory and source root directory

