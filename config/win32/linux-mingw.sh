#!/bin/sh

# $Id$
# script to smoke test build of binaries on Linux
# Phil 2020/09/28

# created on Ubuntu using:
# gcc-mingw-w64-x86-64 mingw-w64-x86-64-dev g++-mingw-w64-x86-64

TOOLCHAIN=x86_64-w64-mingw32-
DEFS=-D_WIN64
CROSS_CONFIG=config.sno.cross
cp -f config/win32/config.sno $CROSS_CONFIG
cat >>$CROSS_CONFIG <<EOF

* override defaults from native (Linux) snobol4 binary!!
	CC = '$(TOOLCHAIN)gcc $(DEFS)'
* speed up sqlite3 compile!
	COPT = ''
	DL_EXT = '.dll'
	DL_LD = CC
	mingw = 1

* attempt to find clock_gettime()
*	TIME_LDFLAGS = '-lpthread'
*	RANDOM_LDFLAGS = '-lpthread'
EOF

make -f config/win32/mingw.mak \
     TOOLCHAIN=x86_64-w64-mingw32- \
     DEFS=-D_WIN64 \
     TCC=gcc \
     OPT=-O \
     CONFIG_SNO=$CROSS_CONFIG \
     MOD_SNOBOL4=snobol4 \
	$*
