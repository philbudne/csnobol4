# $Id$

# make file for MINGW - P. Budne 2/14/2002
# from nmake file for VC++ 5.0 on WinNT 4.0 P. Budne 2/4/1998
# from batch file by David Feustel

# XXX use to define SNOLIB_DIR??
DEST=\snobol4

CC=gcc
# includes -finline-functions (and others in gcc v3)
OPT=-O3 -g

# can also use msdos version (less friendly in multitasking env)
TTY_C=$(SRCDIR)lib/win32/tty.c

# crocks for winsock I/O on Win9x
INET_DEFS=-DINET_IO
# wsock32 present on both Win95 and WinNT
INET_LIBS=-lwsock32

# to disable COM comment out next 3 lines:
COM_LIBS=-lole32 -luuid -loleaut32
COM_DEFS=-DPML_COM
COM_OBJ=com.o

CFLAGS=	-c $(OPT) -I$(SRCDIR)config/win32 -I$(SRCDIR)include -I$(SRCDIR). \
	-DHAVE_CONFIG_H $(INET_DEFS) $(COM_DEFS)

# for com.cpp
CXXFLAGS=$(CFLAGS) -fvtable-thunks

OBJ=	isnobol4.o data.o data_init.o main.o syn.o version.o bal.o \
	date.o dump.o endex.o hash.o intspc.o io.o lexcmp.o ordvst.o \
	pair.o pat.o pml.o realst.o replace.o str.o stream.o top.o \
	tree.o bcopy.o bzero.o dynamic.o expops.o getopt.o init.o \
	load.o mstime.o chop.o cos.o delete.o environ.o exit.o exp.o \
	file.o getstring.o host.o log.o logic.o ord.o rename.o \
	retstring.o sin.o spcint.o spreal.o sprintf.o sqrt.o sset.o \
	tan.o osopen.o sys.o tty.o inet.o bindresvport.o execute.o \
	exists.o term.o findunit.o $(COM_OBJ)

snobol4.exe: $(OBJ)
	$(CC) -o snobol4 $(OBJ) $(INET_LIBS) $(COM_LIBS)

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

version.o: $(SRCDIR)version.c
	$(CC) $(CFLAGS) $(SRCDIR)version.c

################ common

bal.o:	$(SRCDIR)lib/bal.c
	$(CC) $(CFLAGS) $(SRCDIR)lib/bal.c

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

tty.o:	$(TTY_C)
	$(CC) $(CFLAGS) $(TTY_C)

exists.o: $(SRCDIR)lib/win32/exists.c
	$(CC) $(CFLAGS) $(SRCDIR)lib/win32/exists.c

################ snolib

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

host.o:	$(SRCDIR)lib/snolib/host.c
	$(CC) $(CFLAGS) $(SRCDIR)lib/snolib/host.c

log.o:	$(SRCDIR)lib/snolib/log.c
	$(CC) $(CFLAGS) $(SRCDIR)lib/snolib/log.c

logic.o: $(SRCDIR)lib/snolib/logic.c
	$(CC) $(CFLAGS) $(SRCDIR)lib/snolib/logic.c

ord.o:	$(SRCDIR)lib/snolib/ord.c
	$(CC) $(CFLAGS) $(SRCDIR)lib/snolib/ord.c

rename.o: $(SRCDIR)lib/snolib/rename.c
	$(CC) $(CFLAGS) $(SRCDIR)lib/snolib/rename.c

retstring.o: $(SRCDIR)lib/snolib/retstring.c
	$(CC) $(CFLAGS) $(SRCDIR)lib/snolib/retstring.c

sin.o:	$(SRCDIR)lib/snolib/sin.c
	$(CC) $(CFLAGS) $(SRCDIR)lib/snolib/sin.c

sprintf.o: $(SRCDIR)lib/snolib/sprintf.c
	$(CC) $(CFLAGS) $(SRCDIR)lib/snolib/sprintf.c

sqrt.o:	$(SRCDIR)lib/snolib/sqrt.c
	$(CC) $(CFLAGS) $(SRCDIR)lib/snolib/sqrt.c

sset.o:	$(SRCDIR)lib/snolib/sset.c
	$(CC) $(CFLAGS) $(SRCDIR)lib/snolib/sset.c

tan.o:	$(SRCDIR)lib/snolib/tan.c
	$(CC) $(CFLAGS) $(SRCDIR)lib/snolib/tan.c

com.o:	$(SRCDIR)lib/win32/com.cpp
	$(CXX) $(CXXFLAGS) $(SRCDIR)lib/win32/com.cpp

################################################################

install: snobol4.exe
	config\install.bat win32 $(DEST)
