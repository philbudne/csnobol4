# $Id$

# make file for MINGW - P. Budne 2/14/2002
# tested with MINGW 1.7.33 (w/ gcc 4.8.3)
# from nmake file for VC++ 5.0 on WinNT 4.0 P. Budne 2/4/1998
# from batch file by David Feustel

CC=gcc
# includes -finline-functions (and others in gcc v3)
OPT=-O3 -g

# crocks for winsock I/O on Win9x
INET_DEFS=-DINET_IO
# wsock32 present on both Win95 and WinNT
INET_LIBS=-lwsock32

CFLAGS=	-c $(OPT) -I$(SRCDIR)config/win32 -I$(SRCDIR)include -I$(SRCDIR). \
	-DHAVE_CONFIG_H $(INET_DEFS)

LDFLAGS=-Wl,--out-implib,libsnobol4.a

OBJ=	isnobol4.o data.o data_init.o main.o syn.o bal.o break.o \
	date.o dump.o endex.o hash.o intspc.o io.o lexcmp.o ordvst.o \
	pair.o pat.o pml.o realst.o replace.o str.o stream.o top.o \
	tree.o dynamic.o expops.o getopt.o init.o \
	load.o loadx.o mstime.o atan.o chop.o cos.o delete.o \
	environ.o exit.o exp.o \
	file.o getstring.o handle.o host.o log.o ord.o rename.o \
	retstring.o sin.o spcint.o spreal.o sqrt.o sset.o \
	osopen.o sys.o tan.o tty.o inet.o bindresvport.o \
	execute.o exists.o term.o findunit.o

# requires amalgamation sqlite.[ch] in modules/sqlite3:
ifneq (,$(wildcard modules/sqlite3/sqlite3.[ch]))
SQLITE3=sqlite3
endif

all:	cpuid.exe snobol4.exe mods

cpuid.exe: cpuid.c
	$(CC) -o cpuid cpuid.c

MODULES=com dirs logic ndbm sprintf stat time $(SQLITE3)
mods:	snobol4.exe
	for M in $(MODULES); do \
	    (cd modules/$$M; ../../snobol4 -N -I.. -I../.. -I../../snolib \
		-I../../config/win32 setup.sno build); \
	done

snobol4.exe: always $(OBJ)
	$(CC) -shared-libgcc -o snobol4 $(OBJ) $(INET_LIBS) $(LDFLAGS)

# kill leftovers from cygwin builds!!!
always:
	rm -f config.h config.sno

data.o:	$(SRCDIR)data.c
	$(CC) $(CFLAGS) $(SRCDIR)data.c

data_init.o: $(SRCDIR)data_init.c
	$(CC) $(CFLAGS) $(SRCDIR)data_init.c

isnobol4.o: $(SRCDIR)isnobol4.c
	$(CC) $(CFLAGS) $(SRCDIR)isnobol4.c

main.o:	$(SRCDIR)main.c
	$(CC) $(CFLAGS) $(SRCDIR)main.c

syn.o:	$(SRCDIR)syn.c
	$(CC) $(CFLAGS) $(SRCDIR)syn.c

################ common

bal.o:	$(SRCDIR)lib/bal.c
	$(CC) $(CFLAGS) $(SRCDIR)lib/bal.c

break.o: $(SRCDIR)lib/break.c
	$(CC) $(CFLAGS) $(SRCDIR)lib/break.c

date.o:	$(SRCDIR)lib/date.c
	$(CC) $(CFLAGS) $(SRCDIR)lib/date.c

dump.o:	$(SRCDIR)lib/dump.c
	$(CC) $(CFLAGS) $(SRCDIR)lib/dump.c

endex.o: $(SRCDIR)lib/endex.c
	$(CC) $(CFLAGS) $(SRCDIR)lib/endex.c

hash.o:	$(SRCDIR)lib/hash.c
	$(CC) $(CFLAGS) $(SRCDIR)lib/hash.c

init.o:	$(SRCDIR)lib/init.c
	$(CC) $(CFLAGS) $(SRCDIR)lib/init.c

io.o:	$(SRCDIR)lib/io.c
	$(CC) $(CFLAGS) $(SRCDIR)lib/io.c

lexcmp.o: $(SRCDIR)lib/lexcmp.c
	$(CC) $(CFLAGS) $(SRCDIR)lib/lexcmp.c

loadx.o: $(SRCDIR)lib/loadx.c
	$(CC) $(CFLAGS) $(SRCDIR)lib/loadx.c

ordvst.o: $(SRCDIR)lib/ordvst.c
	$(CC) $(CFLAGS) $(SRCDIR)lib/ordvst.c

pair.o:	$(SRCDIR)lib/pair.c
	$(CC) $(CFLAGS) $(SRCDIR)lib/pair.c

pat.o:	$(SRCDIR)lib/pat.c
	$(CC) $(CFLAGS) $(SRCDIR)lib/pat.c

pml.o:	$(SRCDIR)lib/pml.c
	$(CC) $(CFLAGS) $(SRCDIR)lib/pml.c

realst.o: $(SRCDIR)lib/realst.c
	$(CC) $(CFLAGS) $(SRCDIR)lib/realst.c

replace.o: $(SRCDIR)lib/replace.c
	$(CC) $(CFLAGS) $(SRCDIR)lib/replace.c

str.o:	$(SRCDIR)lib/str.c
	$(CC) $(CFLAGS) $(SRCDIR)lib/str.c

stream.o: $(SRCDIR)lib/stream.c
	$(CC) $(CFLAGS) $(SRCDIR)lib/stream.c

top.o:	$(SRCDIR)lib/top.c
	$(CC) $(CFLAGS) $(SRCDIR)lib/top.c

tree.o:	$(SRCDIR)lib/tree.c
	$(CC) $(CFLAGS) $(SRCDIR)lib/tree.c

################ ansi

spcint.o: $(SRCDIR)lib/ansi/spcint.c
	$(CC) $(CFLAGS) $(SRCDIR)lib/ansi/spcint.c

spreal.o: $(SRCDIR)lib/ansi/spreal.c
	$(CC) $(CFLAGS) $(SRCDIR)lib/ansi/spreal.c

################ auxil

bcopy.o: $(SRCDIR)lib/auxil/bcopy.c
	$(CC) $(CFLAGS) $(SRCDIR)lib/auxil/bcopy.c

bzero.o: $(SRCDIR)lib/auxil/bzero.c
	$(CC) $(CFLAGS) $(SRCDIR)lib/auxil/bzero.c

getopt.o: $(SRCDIR)lib/auxil/getopt.c
	$(CC) $(CFLAGS) $(SRCDIR)lib/auxil/getopt.c

bindresvport.o: $(SRCDIR)lib/auxil/bindresvport.c
	$(CC) $(CFLAGS) $(SRCDIR)lib/auxil/bindresvport.c

################ dummy

execute.o: $(SRCDIR)lib/dummy/execute.c
	$(CC) $(CFLAGS) $(SRCDIR)lib/dummy/execute.c

################ generic

dynamic.o: $(SRCDIR)lib/generic/dynamic.c
	$(CC) $(CFLAGS) $(SRCDIR)lib/generic/dynamic.c

expops.o: $(SRCDIR)lib/generic/expops.c
	$(CC) $(CFLAGS) $(SRCDIR)lib/generic/expops.c

intspc.o: $(SRCDIR)lib/generic/intspc.c
	$(CC) $(CFLAGS) $(SRCDIR)lib/generic/intspc.c

################ win32!

inet.o:	$(SRCDIR)lib/win32/inet.c
	$(CC) $(CFLAGS) $(SRCDIR)lib/win32/inet.c

load.o:	$(SRCDIR)lib/win32/load.c
	$(CC) $(CFLAGS) $(SRCDIR)lib/win32/load.c

mstime.o: $(SRCDIR)lib/win32/mstime.c
	$(CC) $(CFLAGS) $(SRCDIR)lib/win32/mstime.c

osopen.o: $(SRCDIR)lib/win32/osopen.c
	$(CC) $(CFLAGS) $(SRCDIR)lib/win32/osopen.c

sys.o:	$(SRCDIR)lib/win32/sys.c
	$(CC) $(CFLAGS) $(SRCDIR)lib/win32/sys.c

term.o:	$(SRCDIR)lib/win32/term.c
	$(CC) $(CFLAGS) $(SRCDIR)lib/win32/term.c

tty.o:	$(SRCDIR)lib/win32/tty.c
	$(CC) $(CFLAGS) $(SRCDIR)lib/win32/tty.c

exists.o: $(SRCDIR)lib/win32/exists.c
	$(CC) $(CFLAGS) $(SRCDIR)lib/win32/exists.c

################ snolib

atan.o:	$(SRCDIR)lib/snolib/atan.c
	$(CC) $(CFLAGS) $(SRCDIR)lib/snolib/atan.c

chop.o:	$(SRCDIR)lib/snolib/chop.c
	$(CC) $(CFLAGS) $(SRCDIR)lib/snolib/chop.c

cos.o:	$(SRCDIR)lib/snolib/cos.c
	$(CC) $(CFLAGS) $(SRCDIR)lib/snolib/cos.c

delete.o: $(SRCDIR)lib/snolib/delete.c
	$(CC) $(CFLAGS) $(SRCDIR)lib/snolib/delete.c

environ.o: $(SRCDIR)lib/snolib/environ.c
	$(CC) $(CFLAGS) $(SRCDIR)lib/snolib/environ.c

exit.o:	$(SRCDIR)lib/snolib/exit.c
	$(CC) $(CFLAGS) $(SRCDIR)lib/snolib/exit.c

exp.o:	$(SRCDIR)lib/snolib/exp.c
	$(CC) $(CFLAGS) $(SRCDIR)lib/snolib/exp.c

file.o:	$(SRCDIR)lib/snolib/file.c
	$(CC) $(CFLAGS) $(SRCDIR)lib/snolib/file.c

findunit.o: $(SRCDIR)lib/snolib/findunit.c
	$(CC) $(CFLAGS) $(SRCDIR)lib/snolib/findunit.c

getstring.o: $(SRCDIR)lib/snolib/getstring.c
	$(CC) $(CFLAGS) $(SRCDIR)lib/snolib/getstring.c

handle.o: $(SRCDIR)lib/snolib/handle.c
	$(CC) $(CFLAGS) $(SRCDIR)lib/snolib/handle.c

host.o:	$(SRCDIR)lib/snolib/host.c
	$(CC) $(CFLAGS) $(SRCDIR)lib/snolib/host.c -DCC=\""$(CC)"\" -DCOPT=\""$(OPT)"\" -DSO_LD=\""$(CC)"\" -DDL_LD=\""$(CC)"\"

log.o:	$(SRCDIR)lib/snolib/log.c
	$(CC) $(CFLAGS) $(SRCDIR)lib/snolib/log.c

ord.o:	$(SRCDIR)lib/snolib/ord.c
	$(CC) $(CFLAGS) $(SRCDIR)lib/snolib/ord.c

rename.o: $(SRCDIR)lib/snolib/rename.c
	$(CC) $(CFLAGS) $(SRCDIR)lib/snolib/rename.c

retstring.o: $(SRCDIR)lib/snolib/retstring.c
	$(CC) $(CFLAGS) $(SRCDIR)lib/snolib/retstring.c

sin.o:	$(SRCDIR)lib/snolib/sin.c
	$(CC) $(CFLAGS) $(SRCDIR)lib/snolib/sin.c

sqrt.o:	$(SRCDIR)lib/snolib/sqrt.c
	$(CC) $(CFLAGS) $(SRCDIR)lib/snolib/sqrt.c

sset.o:	$(SRCDIR)lib/snolib/sset.c
	$(CC) $(CFLAGS) $(SRCDIR)lib/snolib/sset.c

tan.o:	$(SRCDIR)lib/snolib/tan.c
	$(CC) $(CFLAGS) $(SRCDIR)lib/snolib/tan.c
