# $Id$

# make file for DJGPP - P. Budne 2/16/2002
# from make file for MINGW - P. Budne 2/14/2002
# and batch files by Burkhard Meissner
# from nmake file for VC++ 5.0 on WinNT 4.0 P. Budne 2/4/1998
# from batch file by David Feustel

CC=gcc
OPT=-O3

# defaults
INET_C=$(SRCDIR)lib/dummy/inet.c
LOAD_C=$(SRCDIR)lib/dummy/load.c

# DXE2 LOAD() support.  DXE2 available from "v2tk" directory
# on DJGPP distribution mirror sites.  Unpack and run "make install"
#
# uncomment next 4 lines to enable;
#DXE_DEFS=-DHAVE_DXE2
#LOAD_C=$(SRCDIR)lib/unix98/load.c
#DXE_OBJS=osinit.o
#DXE_LIBS=-ldl

# TCP/IP support, using WATTCP (see www.wattcp.com)
#
# edit as needed for your installation;
WATTCP_DIR=/djgpp/wattcp
WATTCP_INCDIR=${WATTCP_DIR}/inc
WATTCP_LIBDIR=${WATTCP_DIR}/lib
#
# uncomment next 4 lines to enable;
#WATTCP_DEFS=-I${WATTCP_INCDIR} -DHAVE_WATTCP
#INET_C=$(SRCDIR)lib/bsd/inet.c
#INET_OBJS=rresvport.o
#WATTCP_LIBS=-L${WATTCP_LIBDIR} -lwatt

CFLAGS=	-c $(OPT) -I$(SRCDIR)config/djgpp -I$(SRCDIR)include -I$(SRCDIR). \
	-DHAVE_CONFIG_H $(WATTCP_DEFS) $(DXE_DEFS)

# WATTCP must be first!
LIBS=$(WATTCP_LIBS) -lm -lc -lpc $(DXE_LIBS) 

OBJ=	isnobol4.o data.o data_init.o main.o syn.o \
	version.o bal.o date.o dump.o endex.o hash.o \
	intspc.o io.o lexcmp.o ordvst.o pair.o pat.o \
	pml.o realst.o replace.o str.o stream.o top.o \
	tree.o bcopy.o bzero.o dynamic.o expops.o getopt.o \
	init.o load.o mstime.o chop.o cos.o delete.o \
	exit.o exp.o file.o getstring.o \
	host.o log.o logic.o ord.o rename.o retstring.o sin.o \
	spcint.o spreal.o sprintf.o sqrt.o sset.o \
	tan.o sys.o tty.o inet.o execute.o exists.o \
	term.o findunit.o $(DXE_OBJS) $(INET_OBJS)

snobol4.exe: $(OBJ)
	$(CC) -o snobol4 $(OBJ) $(LIBS)

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

################ dummy

execute.o: $(SRCDIR)lib/dummy/execute.c
	$(CC) $(CFLAGS) $(SRCDIR)lib/dummy/execute.c

rresvport.o: $(SRCDIR)lib/dummy/rresvport.c
	$(CC) $(CFLAGS) $(SRCDIR)lib/dummy/rresvport.c

################ generic

dynamic.o: $(SRCDIR)lib/generic/dynamic.c
	$(CC) $(CFLAGS) $(SRCDIR)lib/generic/dynamic.c

expops.o: $(SRCDIR)lib/generic/expops.c
	$(CC) $(CFLAGS) $(SRCDIR)lib/generic/expops.c

intspc.o: $(SRCDIR)lib/generic/intspc.c
	$(CC) $(CFLAGS) $(SRCDIR)lib/generic/intspc.c

################ posix!

term.o:	$(SRCDIR)lib/posix/term.c
	$(CC) $(CFLAGS) $(SRCDIR)lib/posix/term.c

mstime.o: $(SRCDIR)lib/posix/mstime.c
	$(CC) $(CFLAGS) $(SRCDIR)lib/posix/mstime.c

tty.o:	$(SRCDIR)lib/posix/tty.c
	$(CC) $(CFLAGS) $(SRCDIR)lib/posix/tty.c

################ misc

# bsd or dummy
inet.o:	$(INET_C)
	$(CC) $(CFLAGS) $(INET_C)

# unix98 or dummy
load.o:	$(LOAD_C)
	$(CC) $(CFLAGS) $(LOAD_C)

################ generic

exists.o: $(SRCDIR)lib/generic/exists.c
	$(CC) $(CFLAGS) $(SRCDIR)lib/generic/exists.c

################ snolib

chop.o:	$(SRCDIR)lib/snolib/chop.c
	$(CC) $(CFLAGS) $(SRCDIR)lib/snolib/chop.c

cos.o:	$(SRCDIR)lib/snolib/cos.c
	$(CC) $(CFLAGS) $(SRCDIR)lib/snolib/cos.c

delete.o: $(SRCDIR)lib/snolib/delete.c
	$(CC) $(CFLAGS) $(SRCDIR)lib/snolib/delete.c

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

################ djgpp

osinit.o: $(SRCDIR)lib/djgpp/osinit.c
	$(CC) $(CFLAGS) $(SRCDIR)lib/djgpp/osinit.c

sys.o:	$(SRCDIR)lib/djgpp/sys.c
	$(CC) $(CFLAGS) $(SRCDIR)lib/djgpp/sys.c

################################################################

install:
	config/djgpp/install.bat
