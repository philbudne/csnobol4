#!/bin/sh

# $Id$
# script to smoke test build of binaries on Linux
# Phil 2020/09/28

# created on Ubuntu using:
# gcc-mingw-w64-x86-64 mingw-w64-x86-64-dev g++-mingw-w64-x86-64


CROSS_CONFIG=config.sno.cross
cp -f config/win32/config.sno $CROSS_CONFIG
cat >>$CROSS_CONFIG <<EOF

* override defaults from native (Linux) snobol4 binary!!
	CC = 'x86_64-w64-mingw32-gcc -D_WIN64'
* speed up sqlite3 compile!
	COPT = ''
	DL_EXT = '.dll'
	DL_LD = CC
	mingw = 1
EOF

make -f config/win32/mingw.mak \
     CC=x86_64-w64-mingw32-gcc \
     OPT=-O \
     CONFIG_SNO=$CROSS_CONFIG \
     MOD_SNOBOL4=snobol4 \
	$*
