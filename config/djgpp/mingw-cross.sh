#!/bin/sh

# try to cross-compile under Cygwin-w64
# using djgpp-gcc-core 5.4.0-1

if grep -q BLOCKS with; then
    export BLOCKS=1
fi
#export WATTCP_DIR=/home/User/Downloads/watt32
#export HAVE_WATTCP=1
make -f config/djgpp/Makefile TOOLCHAIN=i586-pc-msdosdjgpp- $*
