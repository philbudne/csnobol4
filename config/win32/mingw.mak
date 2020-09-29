# $Id$

# make file for MINGW - P. Budne 2/14/2002
# tested (under Linux) with MINGW gcc 9.3 9/27/2020
#	w/ mingw-w64-common 7.0.0-2 
# from nmake file for VC++ 5.0 on WinNT 4.0 P. Budne 2/4/1998
# from batch file by David Feustel

CC=gcc
# includes -finline-functions (and others in gcc v3):
OPT=-O3 -g

# Winsock version: define as 1 or 2.
# comment out for no inet support
# Winsock v1 available on Win95 and NT 3.5
# Winsock v2 available on Win98 and NT 4.0
WINSOCK=2

# not in mingw-w64-common 7.0.0-2 headers
# need STARTUPINFOEX, HPCON, PROC_THREAD_ATTRIBUTE_PSEUDOCONSOLE
PTYIO=1

################################################################

ifdef WINSOCK
INET_DEFS=-DINET_IO
INET_AUX=inetio_obj.o bindresvport.o
INET_AUX_SRC = $(SRCDIR)lib/win32/inetio_obj.c \
	$(SRCDIR)lib/auxil/bindresvport.c
BUFIO_OBJ_O=bufio_obj.o
ifeq ($(WINSOCK),1)
# wsock32 present on both Win95 and WinNT 3.x
INET_C=$(SRCDIR)lib/win32/inet.c
INET_DEFS += -DHAVE_WINSOCK_H
INET_O=inet.o
INET_LIBS=-lwsock32
else
# Winsock2 present on both Win98 and NT 4.0
INET_C=$(SRCDIR)lib/bsd/inet6.c
INET_DEFS += -DHAVE_WINSOCK2_H
INET_O=inet6.o
INET_LIBS=-lws2_32
endif
else
INET_O=inet.o
INET_C=$(SRCDIR)lib/dummy/inet.c
endif

ifdef PTYIO
BUFIO_OBJ_O=bufio_obj.o
PTYIO_OBJ_C=$(SRCDIR)lib/win32/ptyio_obj.c
else
PTYIO_OBJ_C=$(SRCDIR)lib/dummy/ptyio_obj.c
endif

CONFIG_SNO=config/win32/config.sno.mingw

CFLAGS=-c $(OPT) -I$(SRCDIR)config/win32 -I$(SRCDIR)include -I$(SRCDIR). \
	-DHAVE_CONFIG_H $(INET_DEFS) -Wall
SNOBOL4_CFLAGS=$(CFLAGS) -Wno-return-type -Wno-switch

LDFLAGS=-Wl,--out-implib,libsnobol4.a

OBJ=	$(BUFIO_OBJ_O) $(INET_O) $(INET_AUX) atan.o bal.o break.o \
	chop.o cos.o data.o data_init.o date.o delete.o dump.o \
	dynamic.o endex.o environ.o execute.o exists.o \
	exit.o exp.o expops.o file.o findunit.o getline.o getopt.o \
	getstring.o handle.o hash.o host.o init.o intspc.o \
	io.o isnobol4.o lexcmp.o load.o loadx.o log.o main.o \
	mstime.o ord.o ordvst.o osopen.o pair.o pat.o pml.o \
	ptyio_obj.o realst.o rename.o replace.o retstring.o \
	sin.o spcint.o spreal.o sqrt.o sset.o stdio_obj.o \
	str.o stream.o syn.o sys.o tan.o term.o top.o tree.o \
	tty.o

SRC=	$(INET_AUX_SRC) $(INET_C) $(PTYIO_OBJ_C) $(SRCDIR)$(INET_C) \
	$(SRCDIR)$(PTYIO_OBJ_C) $(SRCDIR)data.c $(SRCDIR)data_init.c \
	$(SRCDIR)isnobol4.c $(SRCDIR)lib/ansi/spcint.c \
	$(SRCDIR)lib/ansi/spreal.c $(SRCDIR)lib/auxil/bcopy.c \
	$(SRCDIR)lib/auxil/bzero.c \
	$(SRCDIR)lib/auxil/getline.c $(SRCDIR)lib/auxil/getopt.c \
	$(SRCDIR)lib/bal.c $(SRCDIR)lib/break.c $(SRCDIR)lib/date.c \
	$(SRCDIR)lib/dummy/execute.c $(SRCDIR)lib/dump.c \
	$(SRCDIR)lib/endex.c $(SRCDIR)lib/generic/dynamic.c \
	$(SRCDIR)lib/generic/expops.c $(SRCDIR)lib/generic/intspc.c \
	$(SRCDIR)lib/hash.c $(SRCDIR)lib/init.c $(SRCDIR)lib/io.c \
	$(SRCDIR)lib/lexcmp.c $(SRCDIR)lib/loadx.c \
	$(SRCDIR)lib/ordvst.c $(SRCDIR)lib/pair.c $(SRCDIR)lib/pat.c \
	$(SRCDIR)lib/pml.c $(SRCDIR)lib/realst.c \
	$(SRCDIR)lib/replace.c $(SRCDIR)lib/snolib/atan.c \
	$(SRCDIR)lib/snolib/chop.c $(SRCDIR)lib/snolib/cos.c \
	$(SRCDIR)lib/snolib/delete.c $(SRCDIR)lib/snolib/environ.c \
	$(SRCDIR)lib/snolib/exit.c $(SRCDIR)lib/snolib/exp.c \
	$(SRCDIR)lib/snolib/file.c $(SRCDIR)lib/snolib/findunit.c \
	$(SRCDIR)lib/snolib/getstring.c $(SRCDIR)lib/snolib/handle.c \
	$(SRCDIR)lib/snolib/host.c $(SRCDIR)lib/snolib/log.c \
	$(SRCDIR)lib/snolib/ord.c $(SRCDIR)lib/snolib/rename.c \
	$(SRCDIR)lib/snolib/retstring.c $(SRCDIR)lib/snolib/sin.c \
	$(SRCDIR)lib/snolib/sqrt.c $(SRCDIR)lib/snolib/sset.c \
	$(SRCDIR)lib/snolib/tan.c $(SRCDIR)lib/stdio_obj.c \
	$(SRCDIR)lib/str.c $(SRCDIR)lib/stream.c $(SRCDIR)lib/top.c \
	$(SRCDIR)lib/tree.c $(SRCDIR)lib/win32/exists.c \
	$(SRCDIR)lib/win32/load.c $(SRCDIR)lib/win32/mstime.c \
	$(SRCDIR)lib/win32/osopen.c $(SRCDIR)lib/win32/sys.c \
	$(SRCDIR)lib/win32/term.c $(SRCDIR)lib/win32/tty.c \
	$(SRCDIR)main.c $(SRCDIR)syn.c

ifdef BUFIO_OBJ_O
SRC += $(SRCDIR)lib/auxil/bufio_obj.c
endif

# requires amalgamation sqlite3.[ch] in modules/sqlite3:
ifneq (,$(wildcard modules/sqlite3/sqlite3.[ch]))
SQLITE3=sqlite3
endif

DEPEND=depend.mingw

# make DEPEND file (once) and recurse
all:	$(DEPEND)
	$(MAKE) -f config/win32/mingw.mak snobol4.exe mods

cpuid.exe: cpuid.c
	$(CC) -o cpuid cpuid.c

MODULES=com dirs logic ndbm sprintf stat time $(SQLITE3)

# SNOBOL4 binary to use to build modules (override on cross builds!)
MOD_SNOBOL4=../../snobol4

mods:	snobol4.exe
	for M in $(MODULES); do \
	  (cd modules/$$M; \
	   $(MOD_SNOBOL4) -N -I.. -I../.. -I../../snolib setup.sno build); \
	done

snobol4.exe: always $(OBJ)
	$(CC) -shared-libgcc -o snobol4 $(OBJ) $(INET_LIBS) $(LDFLAGS)

# kill leftovers from cygwin builds!!!
# CONFIG_SNO can be overridden for cross-platform builds
always:
	rm -f config.h
	cp -f $(CONFIG_SNO) config.sno

data.o:	$(SRCDIR)data.c
	$(CC) $(CFLAGS) $(SRCDIR)data.c

data_init.o: $(SRCDIR)data_init.c
	$(CC) $(CFLAGS) $(SRCDIR)data_init.c

isnobol4.o: $(SRCDIR)isnobol4.c
	$(CC) $(SNOBOL4_CFLAGS) $(SRCDIR)isnobol4.c

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

stdio_obj.o:	$(SRCDIR)lib/stdio_obj.c
	$(CC) $(CFLAGS) $(SRCDIR)lib/stdio_obj.c

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

getline.o: $(SRCDIR)lib/auxil/getline.c
	$(CC) $(CFLAGS) $(SRCDIR)lib/auxil/getline.c

getopt.o: $(SRCDIR)lib/auxil/getopt.c
	$(CC) $(CFLAGS) $(SRCDIR)lib/auxil/getopt.c

bindresvport.o: $(SRCDIR)lib/auxil/bindresvport.c
	$(CC) $(CFLAGS) $(SRCDIR)lib/auxil/bindresvport.c

bufio_obj.o: $(SRCDIR)lib/auxil/bufio_obj.c
	$(CC) $(CFLAGS) $(SRCDIR)lib/auxil/bufio_obj.c

################ dummy

execute.o: $(SRCDIR)lib/dummy/execute.c
	$(CC) $(CFLAGS) $(SRCDIR)lib/dummy/execute.c

# dummy version for now (not in MINGW include files)
ptyio_obj.o: $(SRCDIR)$(PTYIO_OBJ_C)
	$(CC) $(CFLAGS) $(SRCDIR)$(PTYIO_OBJ_C)

################ generic

dynamic.o: $(SRCDIR)lib/generic/dynamic.c
	$(CC) $(CFLAGS) $(SRCDIR)lib/generic/dynamic.c

expops.o: $(SRCDIR)lib/generic/expops.c
	$(CC) $(CFLAGS) $(SRCDIR)lib/generic/expops.c

intspc.o: $(SRCDIR)lib/generic/intspc.c
	$(CC) $(CFLAGS) $(SRCDIR)lib/generic/intspc.c

################ win32 or bsd!!

$(INET_O): $(INET_C)
	$(CC) $(CFLAGS) $(INET_C)

################ win32!

inetio_obj.o:	$(SRCDIR)lib/win32/inetio_obj.c
	$(CC) $(CFLAGS) $(SRCDIR)lib/win32/inetio_obj.c

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

################ housekeeping

mod_clean:
	for M in $(MODULES); do \
	  (cd modules/$$M; \
	   $(MOD_SNOBOL4) -N -I.. -I../.. -I../../snolib setup.sno clean); \
	done

clean:	mod_clean
	-rm -f *.o *.exe libsnobol4.a $(DEPEND)

$(DEPEND):
	@echo Depending...
	-$(CC) -MM -MG $(CFLAGS) $(SRC) > depend.tmp
	mv depend.tmp $(DEPEND)

ifneq ($(wildcard $(DEPEND)),)
include $(DEPEND)
endif
