# $Id$

# Win95 Borland BCC32.EXE V5.5 compatible make file by Greg White 9/2/2000
# should also work for WinNT 
#
# from ntmsvc.mak; nmake file for VC++ 5.0 on WinNT 4.0 by P. Budne 2/4/1998
# from batch file by David Feustel

# XXX use to define SNOLIB_DIR??
DEST=\snobol4

CC=bcc32.exe

# -O2 Generate fastest possible code
# -O1 Generate smallest possible code
OPT=-O2

# XXX try enabling bitfields? (comment out next line)
NO_BITFIELDS=-DNO_BITFIELDS

# can also use msdos version (less friendly in multitasking env)
TTY_C=$(SRCDIR)lib\win32\tty.c

# crocks for winsock I/O on Win9x
INET_DEFS=-DINET_IO
# wsock32 present on both Win95 and WinNT
INET_LIBS=wsock32.lib

CFLAGS= -c $(OPT) -WC -Tml -w- \
	-I$(SRCDIR)config\win32 -I$(SRCDIR)include -I$(SRCDIR). \
	-DHAVE_CONFIG_H $(NO_BITFIELDS) $(INET_DEFS)

OBJ=	isnobol4.obj data.obj data_init.obj main.obj syn.obj \
	version.obj bal.obj date.obj dump.obj endex.obj hash.obj \
	intspc.obj io.obj lexcmp.obj ordvst.obj pair.obj pat.obj \
	pml.obj realst.obj replace.obj str.obj stream.obj top.obj \
	tree.obj bcopy.obj bzero.obj dynamic.obj expops.obj getopt.obj \
	init.obj load.obj mstime.obj chop.obj cos.obj delete.obj \
	environ.obj exit.obj file.obj getstring.obj host.obj log.obj \
	logic.obj ord.obj rename.obj retstring.obj sin.obj spcint.obj \
	spreal.obj sqrt.obj sset.obj tan.obj sys.obj tty.obj inet.obj \
	bindresvport.obj execute.obj exists.obj term.obj findunit.obj exp.obj

# Ensure Psdk directory (containing wsock32.lib) is included in the library
# search path in both BCC32.CFG and ILINK.CFG , both found in BCC55\BIN\

AllFiles: $(OBJ) snobol4.exe

snobol4.exe : $(OBJ)
       $(CC) -WC -esnobol4.exe $(OBJ) $(INET_LIBS)

data.obj : $(SRCDIR)data.c
	$(CC) $(CFLAGS) $(SRCDIR)data.c

data_init.obj : $(SRCDIR)data_init.c
	$(CC) $(CFLAGS) $(SRCDIR)data_init.c

isnobol4.obj : $(SRCDIR)isnobol4.c
	$(CC) $(CFLAGS) $(SRCDIR)isnobol4.c

main.obj : $(SRCDIR)main.c
	$(CC) $(CFLAGS) $(SRCDIR)main.c

syn.obj : $(SRCDIR)syn.c
	$(CC) $(CFLAGS) $(SRCDIR)syn.c

version.obj : $(SRCDIR)version.c
	$(CC) $(CFLAGS) $(SRCDIR)version.c

################ common

bal.obj : $(SRCDIR)lib\bal.c
	$(CC) $(CFLAGS) $(SRCDIR)lib\bal.c

date.obj : $(SRCDIR)lib\date.c
	$(CC) $(CFLAGS) $(SRCDIR)lib\date.c

dump.obj : $(SRCDIR)lib\dump.c
	$(CC) $(CFLAGS) $(SRCDIR)lib\dump.c

endex.obj : $(SRCDIR)lib\endex.c
	$(CC) $(CFLAGS) $(SRCDIR)lib\endex.c

hash.obj : $(SRCDIR)lib\hash.c
	$(CC) $(CFLAGS) $(SRCDIR)lib\hash.c

init.obj : $(SRCDIR)lib\init.c
	$(CC) $(CFLAGS) $(SRCDIR)lib\init.c

io.obj : $(SRCDIR)lib\io.c
	$(CC) $(CFLAGS) $(SRCDIR)lib\io.c

lexcmp.obj : $(SRCDIR)lib\lexcmp.c
	$(CC) $(CFLAGS) $(SRCDIR)lib\lexcmp.c

ordvst.obj : $(SRCDIR)lib\ordvst.c
	$(CC) $(CFLAGS) $(SRCDIR)lib\ordvst.c

pair.obj : $(SRCDIR)lib\pair.c
	$(CC) $(CFLAGS) $(SRCDIR)lib\pair.c

pat.obj : $(SRCDIR)lib\pat.c
	$(CC) $(CFLAGS) $(SRCDIR)lib\pat.c

pml.obj : $(SRCDIR)lib\pml.c
	$(CC) $(CFLAGS) $(SRCDIR)lib\pml.c

realst.obj : $(SRCDIR)lib\realst.c
	$(CC) $(CFLAGS) $(SRCDIR)lib\realst.c

replace.obj : $(SRCDIR)lib\replace.c
	$(CC) $(CFLAGS) $(SRCDIR)lib\replace.c

str.obj : $(SRCDIR)lib\str.c
	$(CC) $(CFLAGS) $(SRCDIR)lib\str.c

stream.obj : $(SRCDIR)lib\stream.c
	$(CC) $(CFLAGS) $(SRCDIR)lib\stream.c

top.obj : $(SRCDIR)lib\top.c
	$(CC) $(CFLAGS) $(SRCDIR)lib\top.c

tree.obj : $(SRCDIR)lib\tree.c
	$(CC) $(CFLAGS) $(SRCDIR)lib\tree.c

################ ansi

spcint.obj : $(SRCDIR)lib\ansi\spcint.c
	$(CC) $(CFLAGS) $(SRCDIR)lib\ansi\spcint.c

spreal.obj : $(SRCDIR)lib\ansi\spreal.c
	$(CC) $(CFLAGS) $(SRCDIR)lib\ansi\spreal.c

################ auxil

bcopy.obj : $(SRCDIR)lib\auxil\bcopy.c
	$(CC) $(CFLAGS) $(SRCDIR)lib\auxil\bcopy.c

bzero.obj : $(SRCDIR)lib\auxil\bzero.c
	$(CC) $(CFLAGS) $(SRCDIR)lib\auxil\bzero.c

getopt.obj : $(SRCDIR)lib\auxil\getopt.c
	$(CC) $(CFLAGS) $(SRCDIR)lib\auxil\getopt.c

bindresvport.obj : $(SRCDIR)lib\auxil\bindresvport.c
	$(CC) $(CFLAGS) $(SRCDIR)lib\auxil\bindresvport.c

################ dummy

execute.obj : $(SRCDIR)lib\dummy\execute.c
	$(CC) $(CFLAGS) $(SRCDIR)lib\dummy\execute.c

################ generic

dynamic.obj : $(SRCDIR)lib\generic\dynamic.c
	$(CC) $(CFLAGS) $(SRCDIR)lib\generic\dynamic.c

expops.obj : $(SRCDIR)lib\generic\expops.c
	$(CC) $(CFLAGS) $(SRCDIR)lib\generic\expops.c

intspc.obj : $(SRCDIR)lib\generic\intspc.c
	$(CC) $(CFLAGS) $(SRCDIR)lib\generic\intspc.c

################ win32!

inet.obj : $(SRCDIR)lib\win32\inet.c
	$(CC) $(CFLAGS) $(SRCDIR)lib\win32\inet.c

load.obj : $(SRCDIR)lib\win32\load.c
	$(CC) $(CFLAGS) $(SRCDIR)lib\win32\load.c

mstime.obj : $(SRCDIR)lib\win32\mstime.c
	$(CC) $(CFLAGS) $(SRCDIR)lib\win32\mstime.c

sys.obj : $(SRCDIR)lib\win32\sys.c
	$(CC) $(CFLAGS) $(SRCDIR)lib\win32\sys.c

term.obj : $(SRCDIR)lib\win32\term.c
	$(CC) $(CFLAGS) $(SRCDIR)lib\win32\term.c

tty.obj : $(TTY_C)
	$(CC) $(CFLAGS) $(TTY_C)

exists.obj : $(SRCDIR)lib\win32\exists.c
	$(CC) $(CFLAGS) $(SRCDIR)lib\win32\exists.c

################ snolib

chop.obj : $(SRCDIR)lib\snolib\chop.c
	$(CC) $(CFLAGS) $(SRCDIR)lib\snolib\chop.c

cos.obj : $(SRCDIR)lib\snolib\cos.c
	$(CC) $(CFLAGS) $(SRCDIR)lib\snolib\cos.c

delete.obj : $(SRCDIR)lib\snolib\delete.c
	$(CC) $(CFLAGS) $(SRCDIR)lib\snolib\delete.c

environ.obj : $(SRCDIR)lib\snolib\environ.c
	$(CC) $(CFLAGS) $(SRCDIR)lib\snolib\environ.c

exit.obj : $(SRCDIR)lib\snolib\exit.c
	$(CC) $(CFLAGS) $(SRCDIR)lib\snolib\exit.c

exp.obj : $(SRCDIR)lib\snolib\exp.c
	$(CC) $(CFLAGS) $(SRCDIR)lib\snolib\exp.c

file.obj : $(SRCDIR)lib\snolib\file.c
	$(CC) $(CFLAGS) $(SRCDIR)lib\snolib\file.c

findunit.obj : $(SRCDIR)lib\snolib\findunit.c
	$(CC) $(CFLAGS) $(SRCDIR)lib\snolib\findunit.c

getstring.obj : $(SRCDIR)lib\snolib\getstring.c
	$(CC) $(CFLAGS) $(SRCDIR)lib\snolib\getstring.c

host.obj : $(SRCDIR)lib\snolib\host.c
	$(CC) $(CFLAGS) $(SRCDIR)lib\snolib\host.c

log.obj : $(SRCDIR)lib\snolib\log.c
	$(CC) $(CFLAGS) $(SRCDIR)lib\snolib\log.c

logic.obj : $(SRCDIR)lib\snolib\logic.c
	$(CC) $(CFLAGS) $(SRCDIR)lib\snolib\logic.c

ord.obj : $(SRCDIR)lib\snolib\ord.c
	$(CC) $(CFLAGS) $(SRCDIR)lib\snolib\ord.c

rename.obj : $(SRCDIR)lib\snolib\rename.c
	$(CC) $(CFLAGS) $(SRCDIR)lib\snolib\rename.c

retstring.obj : $(SRCDIR)lib\snolib\retstring.c
	$(CC) $(CFLAGS) $(SRCDIR)lib\snolib\retstring.c

sin.obj : $(SRCDIR)lib\snolib\sin.c
	$(CC) $(CFLAGS) $(SRCDIR)lib\snolib\sin.c

sqrt.obj : $(SRCDIR)lib\snolib\sqrt.c
	$(CC) $(CFLAGS) $(SRCDIR)lib\snolib\sqrt.c

sset.obj : $(SRCDIR)lib\snolib\sset.c
	$(CC) $(CFLAGS) $(SRCDIR)lib\snolib\sset.c

tan.obj : $(SRCDIR)lib\snolib\tan.c
	$(CC) $(CFLAGS) $(SRCDIR)lib\snolib\tan.c

################################################################

install:
	config\install.bat win32 $(DEST)
