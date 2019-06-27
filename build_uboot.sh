#!/bin/sh 
source ./setPath.sh
make distclean
make nuc972_rainbow_defconfig
make  -j4
# file u-boot.bin and u-boot-spl.bin in the spl directory and source root directory

