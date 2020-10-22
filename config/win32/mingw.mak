# $Id$

# make file for MINGW - P. Budne 2/14/2002
# tested (under Linux) with MINGW gcc 9.3 9/27/2020
#	w/ mingw-w64-common 7.0.0-2 
# from nmake file for VC++ 5.0 on WinNT 4.0 P. Budne 2/4/1998
# from batch file by David Feustel

# set TOOLCHAIN= on command line (ie; x86_64-w64-mingw32-)
# for cross-compiles

CC=$(TOOLCHAIN)gcc
WINDRES=$(TOOLCHAIN)windres

# THIS file!
MAKEFILE=config/win32/mingw.mak

# includes -finline-functions (and others in gcc v3):
OPT=-O3 -g

# Winsock version: define as 1 or 2.
# comment out for no inet support
# Winsock v1 available on Win95 and NT 3.5
# Winsock v2 available on Win98 and NT 4.0 (IPv6 available in WinXP?)
WINSOCK=2

# goods are in mingw-w64-common 7.0.0-2 headers
# does dynamic symbol lookup so can run on old systems
PTYIO=1

################################################################

ifdef WINSOCK
INET_DEFS=-DINET_IO
INET_AUX=inetio_obj.o bindresvport.o
INET_AUX_SRC = $(SRCDIR)lib/win32/inetio_obj.c \
	$(SRCDIR)lib/auxil/bindresvport.c
NEED_BUFIO=1
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
NEED_BUFIO=1
PTYIO_OBJ_C=$(SRCDIR)lib/win32/ptyio_obj.c
else
PTYIO_OBJ_C=$(SRCDIR)lib/dummy/ptyio_obj.c
endif

CONFIG_SNO=config/win32/config.sno

# DEFS set on command line for cross-builds, as needed
CFLAGS=-c $(OPT) -I$(SRCDIR)config/win32 -I$(SRCDIR)include -I$(SRCDIR). \
	-DHAVE_CONFIG_H $(INET_DEFS) $(DEFS) -DSNOBOL4 -Wall
SNOBOL4_CFLAGS=$(CFLAGS) -Wno-return-type -Wno-switch
HOST_CFLAGS=$(CFLAGS) -DCC=\"$(TCC)\" -DCOPT=\"$(OPT)\" -DSO_LD=\"$(TCC)\" -DDL_LD=\"$(TCC)\"

# target C compiler (overridden for cross-compiles)
TCC=$(CC)

# maybe less confusing if this was snobol4imp?
# but snobol4.lib is the default output w/ MSVC
# -lsnobol4 built into setuputil.sno XXX should honor a config.sno setting
IMPLIB=snobol4.lib
LDFLAGS=-Wl,--out-implib,$(IMPLIB)

SRC=	$(BUFIO_OBJ_C) $(INET_AUX_SRC) $(INET_C) $(PTYIO_OBJ_C) \
	$(SRCDIR)data.c $(SRCDIR)data_init.c \
	$(SRCDIR)lib/ansi/spcint.c $(SRCDIR)lib/ansi/spreal.c \
	$(SRCDIR)lib/auxil/getline.c $(SRCDIR)lib/auxil/getopt.c \
	$(SRCDIR)lib/bal.c $(SRCDIR)lib/break.c \
	$(SRCDIR)lib/date.c $(SRCDIR)lib/dump.c \
	$(SRCDIR)lib/endex.c $(SRCDIR)lib/generic/dynamic.c \
	$(SRCDIR)lib/generic/expops.c $(SRCDIR)lib/generic/intspc.c \
	$(SRCDIR)lib/generic/newer.c $(SRCDIR)lib/hash.c \
	$(SRCDIR)lib/init.c $(SRCDIR)lib/io.c \
	$(SRCDIR)lib/lexcmp.c $(SRCDIR)lib/loadx.c \
	$(SRCDIR)lib/ordvst.c $(SRCDIR)lib/pair.c $(SRCDIR)lib/pat.c \
	$(SRCDIR)lib/pml.c $(SRCDIR)lib/realst.c \
	$(SRCDIR)lib/replace.c $(SRCDIR)lib/snolib/atan.c \
	$(SRCDIR)lib/snolib/chop.c $(SRCDIR)lib/snolib/cos.c \
	$(SRCDIR)lib/snolib/delete.c $(SRCDIR)lib/snolib/environ.c \
	$(SRCDIR)lib/snolib/exit.c $(SRCDIR)lib/snolib/exp.c \
	$(SRCDIR)lib/snolib/file.c $(SRCDIR)lib/snolib/findunit.c \
	$(SRCDIR)lib/snolib/getstring.c $(SRCDIR)lib/snolib/handle.c \
	$(SRCDIR)lib/snolib/log.c \
	$(SRCDIR)lib/snolib/ord.c $(SRCDIR)lib/snolib/rename.c \
	$(SRCDIR)lib/snolib/retstring.c $(SRCDIR)lib/snolib/sin.c \
	$(SRCDIR)lib/snolib/sqrt.c $(SRCDIR)lib/snolib/sset.c \
	$(SRCDIR)lib/snolib/tan.c $(SRCDIR)lib/stdio_obj.c \
	$(SRCDIR)lib/str.c $(SRCDIR)lib/stream.c $(SRCDIR)lib/top.c \
	$(SRCDIR)lib/tree.c $(SRCDIR)lib/win32/execute.c \
	$(SRCDIR)lib/win32/exists.c \
	$(SRCDIR)lib/win32/load.c $(SRCDIR)lib/win32/mstime.c \
	$(SRCDIR)lib/win32/osopen.c $(SRCDIR)lib/win32/sys.c \
	$(SRCDIR)lib/win32/term.c $(SRCDIR)lib/win32/tty.c \
	$(SRCDIR)main.c $(SRCDIR)syn.c

ifdef MEMIO
SRC += $(SRCDIR)lib/auxil/memio_obj.c
NEED_BUFIO=1
endif

ifdef NEED_BUFIO
SRC += $(SRCDIR)lib/auxil/bufio_obj.c
endif

# generate list	of baz.o from list of foo/bar/baz.c
OBJ=$(addsuffix .o,$(basename $(notdir $(SRC)))) isnobol4.o host.o

all:	always cpuid.exe snobol4.exe mods

# generated file with explicit rules to make .o files
# only made once, so need to remove if adding a source file,
# or a new nested (ugh) include
RULES=rules.mingw

# NOTE! CFLAGS get baked into RULES file
# (or else need to pass in name of per-file-group CFLAGS)
# so RULES file depends on Makefile (so it's remade)
MAKERULESDEP=$(SRCDIR)config/makerulesdep

$(RULES): $(SRCDIR)$(MAKEFILE)
	CC="$(CC)" CFLAGS='$(CFLAGS)' $(MAKERULESDEP) $(SRC) > $(RULES)
	CC="$(CC)" CFLAGS='$(SNOBOL4_CFLAGS)' $(MAKERULESDEP) \
		$(SRCDIR)isnobol4.c >> $(RULES)
	CC="$(CC)" CFLAGS='$(HOST_CFLAGS)' $(MAKERULESDEP) \
		$(SRCDIR)lib/snolib/host.c >> $(RULES)

# implicitly depends on RULES (above)
include $(RULES)

cpuid.exe: cpuid.c
	$(CC) -o cpuid cpuid.c

snobol4.exe $(IMPLIB): $(OBJ) manifest.o
	$(CC) -shared-libgcc -o snobol4 $(OBJ) manifest.o $(INET_LIBS) $(LDFLAGS)

MANIFEST=$(SRCDIR)config/win32/manifest.rc
manifest.o: $(MANIFEST)
	$(WINDRES) $(MANIFEST) manifest.o

# kill leftovers from cygwin builds!!!
# CONFIG_SNO can be overridden for cross-platform builds
always:
	rm -f config.h
	cp -f $(CONFIG_SNO) config.sno

################ modules

MODULES=base64 dirs logic ndbm random sprintf stat time

# copied from (machine generated) Unix Makefile2:
# (hand added .c sources), removed xsnobol4, generated docs

# requires amalgamation sqlite3.[ch] in modules/sqlite3:
ifneq (,$(wildcard modules/sqlite3/sqlite3.[ch]))
MODULES += sqlite3
endif

ifneq ($(shell uname),Linux)
MODULES += com
endif

mods:	snobol4.exe
	for M in $(MODULES); do \
	  make -C modules/$$M all; \
	done

################ DLL

DLLDIR=dllobj
DLLNAME=snobol4.dll
DLLLIB=snobol4.lib

dll $(DLLDIR)/$(DLLNAME) $(DLLDIR)/$(DLLLIB): always
	-test -d $(DLLDIR) || mkdir $(DLLDIR)
	$(MAKE) -C $(DLLDIR) -f ../$(MAKEFILE) \
		DEFS='-DSHARED' MEMIO=1 SRCDIR=../ dll2

# invoked by above, in DLLDIR, with tweaked variables
# target NOT named snobol4.dll, 'cause someone might try "make snobol4.dll"
# at top level, which would pick up the wrong .obj files

dll2:	$(OBJ) manifest.o
	$(CC) -shared -o $(DLLNAME) $(OBJ) manifest.o $(INET_LIBS) $(LDFLAGS)

# DLL test programs

ssnobol4.exe: ssnobol4.c $(DLLDIR)/$(DLLLIB)
	$(CC) -o ssnobol4.exe -Iinclude ssnobol4.c $(DLLDIR)/$(DLLLIB)

tlib.exe: tlib.c $(DLLDIR)/$(DLLLIB)
	$(CC) -o tlib.exe -Iinclude tlib.c $(DLLDIR)/$(DLLLIB)

################ housekeeping

mod_clean:
	for M in $(MODULES); do \
	  (cd modules/$$M; \
	   $(MOD_SNOBOL4) -N -I.. -I../.. -I../../snolib setup.sno clean); \
	done

clean:	mod_clean
	-rm -f *.o *.exe $(IMPLIB) $(RULES)
	-rm -rf $(DLLDIR)
