# $Id$

# make file for DJGPP - P. Budne 2/16/2002
# from make file for MINGW - P. Budne 2/14/2002
# and batch files by Burkhard Meissner
# from nmake file for VC++ 5.0 on WinNT 4.0 P. Budne 2/4/1998
# from batch file by David Feustel

CC=gcc
OPT=-O2 -finline-functions

# defaults
INET_C=lib\dummy\inet.c
LOAD_C=lib\dummy\load.c

# uncomment next 3 lines to build with WATTCP
#WATTCP_DEFS=-I/djgpp/v2/contrib/wattcp/inc -DWATTCP_DOS
#WATTCP_LIBS=-L/djgpp/v2/lib -lwatt
#INET_C=lib\bsd\inet.c
#INET_OBJS=rresvport.o

# DXE loading
#LOAD_C=lib\unix98\load.c
#DXE_OBJS=osinit.o
#DXE_DEFS=-DHAVE_OSINIT
#DXE_LIBS=-ldl

CFLAGS=	-c $(OPT) -Iconfig/djgpp -Iinclude -I. \
	-DHAVE_CONFIG_H $(WATTCP_DEFS) $(DXE_DEFS)

LIBS=-lm -lc -lpc $(DXE_LIBS) $(WATTCP_LIBS)

OBJ=	isnobol4.o data.o data_init.o main.o syn.o \
	version.o bal.o date.o dump.o endex.o hash.o \
	intspc.o io.o lexcmp.o ordvst.o pair.o pat.o \
	pml.o realst.o replace.o str.o stream.o top.o \
	tree.o bcopy.o bzero.o dynamic.o expops.o getopt.o \
	init.o load.o mstime.o chop.o cos.o delete.o \
	environ.o exit.o exp.o file.o getstring.o \
	host.o log.o logic.o ord.o rename.o retstring.o sin.o \
	spcint.o spreal.o sprintf.o sqrt.o sset.o \
	tan.o sys.o tty.o inet.o execute.o exists.o \
	term.o findunit.o $(DXE_OBJS) $(INET_OBJS)

snobol4.exe : $(OBJ)
	$(CC) -o snobol4 $(OBJ) $(LIBS)

data.o : data.c
	$(CC) $(CFLAGS) data.c

data_init.o : data_init.c
	$(CC) $(CFLAGS) data_init.c

isnobol4.o : isnobol4.c
	$(CC) $(CFLAGS) isnobol4.c

main.o : main.c
	$(CC) $(CFLAGS) main.c

syn.o : syn.c
	$(CC) $(CFLAGS) syn.c

version.o : version.c
	$(CC) $(CFLAGS) version.c

################ common

bal.o : lib\bal.c
	$(CC) $(CFLAGS) lib\bal.c

date.o : lib\date.c
	$(CC) $(CFLAGS) lib\date.c

dump.o : lib\dump.c
	$(CC) $(CFLAGS) lib\dump.c

endex.o : lib\endex.c
	$(CC) $(CFLAGS) lib\endex.c

hash.o : lib\hash.c
	$(CC) $(CFLAGS) lib\hash.c

init.o : lib\init.c
	$(CC) $(CFLAGS) lib\init.c

io.o : lib\io.c
	$(CC) $(CFLAGS) lib\io.c

lexcmp.o : lib\lexcmp.c
	$(CC) $(CFLAGS) lib\lexcmp.c

ordvst.o : lib\ordvst.c
	$(CC) $(CFLAGS) lib\ordvst.c

pair.o : lib\pair.c
	$(CC) $(CFLAGS) lib\pair.c

pat.o : lib\pat.c
	$(CC) $(CFLAGS) lib\pat.c

pml.o : lib\pml.c
	$(CC) $(CFLAGS) lib\pml.c

realst.o : lib\realst.c
	$(CC) $(CFLAGS) lib\realst.c

replace.o : lib\replace.c
	$(CC) $(CFLAGS) lib\replace.c

str.o : lib\str.c
	$(CC) $(CFLAGS) lib\str.c

stream.o : lib\stream.c
	$(CC) $(CFLAGS) lib\stream.c

top.o : lib\top.c
	$(CC) $(CFLAGS) lib\top.c

tree.o : lib\tree.c
	$(CC) $(CFLAGS) lib\tree.c

################ ansi

spcint.o : lib\ansi\spcint.c
	$(CC) $(CFLAGS) lib\ansi\spcint.c

spreal.o : lib\ansi\spreal.c
	$(CC) $(CFLAGS) lib\ansi\spreal.c

################ auxil

bcopy.o : lib\auxil\bcopy.c
	$(CC) $(CFLAGS) lib\auxil\bcopy.c

bzero.o : lib\auxil\bzero.c
	$(CC) $(CFLAGS) lib\auxil\bzero.c

getopt.o : lib\auxil\getopt.c
	$(CC) $(CFLAGS) lib\auxil\getopt.c

################ dummy

execute.o : lib\dummy\execute.c
	$(CC) $(CFLAGS) lib\dummy\execute.c

rresvport.o : lib\dummy\rresvport.c
	$(CC) $(CFLAGS) lib\dummy\rresvport.c

################ generic

dynamic.o : lib\generic\dynamic.c
	$(CC) $(CFLAGS) lib\generic\dynamic.c

expops.o : lib\generic\expops.c
	$(CC) $(CFLAGS) lib\generic\expops.c

intspc.o : lib\generic\intspc.c
	$(CC) $(CFLAGS) lib\generic\intspc.c

################ posix

term.o : lib\posix\term.c
	$(CC) $(CFLAGS) lib\posix\term.c

mstime.o : lib\posix\mstime.c
	$(CC) $(CFLAGS) lib\posix\mstime.c

tty.o : lib\posix\tty.c
	$(CC) $(CFLAGS) lib\posix\tty.c

################ misc

inet.o : $(INET_C)
	$(CC) $(CFLAGS) $(INET_C)

load.o : $(LOAD_C)
	$(CC) $(CFLAGS) $(LOAD_C)

################ generic

exists.o : lib\generic\exists.c
	$(CC) $(CFLAGS) lib\generic\exists.c

################ snolib

chop.o : lib\snolib\chop.c
	$(CC) $(CFLAGS) lib\snolib\chop.c

cos.o : lib\snolib\cos.c
	$(CC) $(CFLAGS) lib\snolib\cos.c

delete.o : lib\snolib\delete.c
	$(CC) $(CFLAGS) lib\snolib\delete.c

environ.o : lib\snolib\environ.c
	$(CC) $(CFLAGS) lib\snolib\environ.c

exit.o : lib\snolib\exit.c
	$(CC) $(CFLAGS) lib\snolib\exit.c

exp.o : lib\snolib\exp.c
	$(CC) $(CFLAGS) lib\snolib\exp.c

file.o : lib\snolib\file.c
	$(CC) $(CFLAGS) lib\snolib\file.c

findunit.o : lib\snolib\findunit.c
	$(CC) $(CFLAGS) lib\snolib\findunit.c

getstring.o : lib\snolib\getstring.c
	$(CC) $(CFLAGS) lib\snolib\getstring.c

host.o : lib\snolib\host.c
	$(CC) $(CFLAGS) lib\snolib\host.c

log.o : lib\snolib\log.c
	$(CC) $(CFLAGS) lib\snolib\log.c

logic.o : lib\snolib\logic.c
	$(CC) $(CFLAGS) lib\snolib\logic.c

ord.o : lib\snolib\ord.c
	$(CC) $(CFLAGS) lib\snolib\ord.c

rename.o : lib\snolib\rename.c
	$(CC) $(CFLAGS) lib\snolib\rename.c

retstring.o : lib\snolib\retstring.c
	$(CC) $(CFLAGS) lib\snolib\retstring.c

sin.o : lib\snolib\sin.c
	$(CC) $(CFLAGS) lib\snolib\sin.c

sprintf.o : lib\snolib\sprintf.c
	$(CC) $(CFLAGS) lib\snolib\sprintf.c

sqrt.o : lib\snolib\sqrt.c
	$(CC) $(CFLAGS) lib\snolib\sqrt.c

sset.o : lib\snolib\sset.c
	$(CC) $(CFLAGS) lib\snolib\sset.c

tan.o : lib\snolib\tan.c
	$(CC) $(CFLAGS) lib\snolib\tan.c

################ djgpp

osinit.o : lib\djgpp\osinit.c
	$(CC) $(CFLAGS) lib\djgpp\osinit.c

sys.o : lib\djgpp\sys.c
	$(CC) $(CFLAGS) lib\djgpp\sys.c
