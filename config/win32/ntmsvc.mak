# $Id$

# nmake file for VC++ 5.0 on WinNT 4.0
# P. Budne 2/4/1998 (from batch file by David Feustel)

CC=cl

# -O2 opt speed
# -Ot favor speed
# -Ox max opt (for space)
OPT=-O2

# XXX try enabling bitfields? (comment out next line)
NO_BITFIELDS=-DNO_BITFIELDS -DFFLD_T=char

# "borland" version of tty routines uses kbhit() spin loop for raw tty
# i/o.  This is unfriendly in a multitasking environment, and should
# be replaced by the win32 version (see below0.
TTY_C=lib\msdos\tty.c
TTY_DEFS=-DTTY_READ_RAW

# win32 tty.c does not yet work.
#TTY_C=lib\win32\tty.c
#TTY_DEFS=

CFLAGS=	-c $(OPT) -Iinclude -I. -Iconfig\win32 -DHAVE_CONFIG_H \
	$(NO_BITFIELDS) $(TTY_DEFS)

OBJ=	snobol4.obj data.obj data_init.obj main.obj syn.obj \
	version.obj bal.obj date.obj dump.obj endex.obj hash.obj \
	intspc.obj io.obj lexcmp.obj ordvst.obj pair.obj pat.obj \
	pml.obj realst.obj replace.obj str.obj stream.obj top.obj \
	tree.obj bcopy.obj bzero.obj dynamic.obj expops.obj getopt.obj \
	init.obj load.obj mstime.obj chop.obj cos.obj delete.obj \
	environ.obj exit.obj file.obj getenv.obj getstring.obj \
	host.obj log.obj logic.obj rename.obj retstring.obj sin.obj \
	spcint.obj spreal.obj sprintf.obj sqrt.obj sset.obj system.obj \
	tan.obj sys.obj popen.obj tty.obj inet.obj execute.obj exists.obj \
	rresvport.obj term.obj findunit.obj exp.obj

# wsock32 present on both Win95 and WinNT
LIBS=wsock32.lib

snobol4.exe : $(OBJ)
	link /out:snobol4.exe $(OBJ) $(LIBS)

data.obj : data.c
	$(CC) $(CFLAGS) data.c

data_init.obj : data_init.c
	$(CC) $(CFLAGS) data_init.c

snobol4.obj : snobol4.c
	$(CC) $(CFLAGS) snobol4.c

main.obj : main.c
	$(CC) $(CFLAGS) main.c

syn.obj : syn.c
	$(CC) $(CFLAGS) syn.c

version.obj : version.c
	$(CC) $(CFLAGS) version.c

################ common

bal.obj : lib\bal.c
	$(CC) $(CFLAGS) lib\bal.c

date.obj : lib\date.c
	$(CC) $(CFLAGS) lib\date.c

dump.obj : lib\dump.c
	$(CC) $(CFLAGS) lib\dump.c

endex.obj : lib\endex.c
	$(CC) $(CFLAGS) lib\endex.c

hash.obj : lib\hash.c
	$(CC) $(CFLAGS) lib\hash.c

init.obj : lib\init.c
	$(CC) $(CFLAGS) lib\init.c

intspc.obj : lib\intspc.c
	$(CC) $(CFLAGS) lib\intspc.c

io.obj : lib\io.c
	$(CC) $(CFLAGS) lib\io.c

lexcmp.obj : lib\lexcmp.c
	$(CC) $(CFLAGS) lib\lexcmp.c

ordvst.obj : lib\ordvst.c
	$(CC) $(CFLAGS) lib\ordvst.c

pair.obj : lib\pair.c
	$(CC) $(CFLAGS) lib\pair.c

pat.obj : lib\pat.c
	$(CC) $(CFLAGS) lib\pat.c

pml.obj : lib\pml.c
	$(CC) $(CFLAGS) lib\pml.c

realst.obj : lib\realst.c
	$(CC) $(CFLAGS) lib\realst.c

replace.obj : lib\replace.c
	$(CC) $(CFLAGS) lib\replace.c

str.obj : lib\str.c
	$(CC) $(CFLAGS) lib\str.c

stream.obj : lib\stream.c
	$(CC) $(CFLAGS) lib\stream.c

top.obj : lib\top.c
	$(CC) $(CFLAGS) lib\top.c

tree.obj : lib\tree.c
	$(CC) $(CFLAGS) lib\tree.c

################ ansi

spcint.obj : lib\ansi\spcint.c
	$(CC) $(CFLAGS) lib\ansi\spcint.c

spreal.obj : lib\ansi\spreal.c
	$(CC) $(CFLAGS) lib\ansi\spreal.c

################ auxil

bcopy.obj : lib\auxil\bcopy.c
	$(CC) $(CFLAGS) lib\auxil\bcopy.c

bzero.obj : lib\auxil\bzero.c
	$(CC) $(CFLAGS) lib\auxil\bzero.c

getopt.obj : lib\auxil\getopt.c
	$(CC) $(CFLAGS) lib\auxil\getopt.c

################ dummy

execute.obj : lib\dummy\execute.c
	$(CC) $(CFLAGS) lib\dummy\execute.c

rresvport.obj : lib\dummy\rresvport.c
	$(CC) $(CFLAGS) lib\dummy\rresvport.c

################ generic

dynamic.obj : lib\generic\dynamic.c
	$(CC) $(CFLAGS) lib\generic\dynamic.c

expops.obj : lib\generic\expops.c
	$(CC) $(CFLAGS) lib\generic\expops.c

term.obj : lib\generic\term.c
	$(CC) $(CFLAGS) lib\generic\term.c

################ win32!

inet.obj : lib\win32\inet.c
	$(CC) $(CFLAGS) lib\win32\inet.c

load.obj : lib\win32\load.c
	$(CC) $(CFLAGS) lib\win32\load.c

mstime.obj : lib\win32\mstime.c
	$(CC) $(CFLAGS) lib\win32\mstime.c

popen.obj : lib\win32\popen.c
	$(CC) $(CFLAGS) lib\win32\popen.c

sys.obj : lib\win32\sys.c
	$(CC) $(CFLAGS) lib\win32\sys.c

tty.obj : $(TTY_C)
	$(CC) $(CFLAGS) $(TTY_C)

exists.obj : lib\win32\exists.c
	$(CC) $(CFLAGS) lib\win32\exists.c

################ snolib

chop.obj : lib\snolib\chop.c
	$(CC) $(CFLAGS) lib\snolib\chop.c

cos.obj : lib\snolib\cos.c
	$(CC) $(CFLAGS) lib\snolib\cos.c

delete.obj : lib\snolib\delete.c
	$(CC) $(CFLAGS) lib\snolib\delete.c

environ.obj : lib\snolib\environ.c
	$(CC) $(CFLAGS) lib\snolib\environ.c

exit.obj : lib\snolib\exit.c
	$(CC) $(CFLAGS) lib\snolib\exit.c

exp.obj : lib\snolib\exp.c
	$(CC) $(CFLAGS) lib\snolib\exp.c

file.obj : lib\snolib\file.c
	$(CC) $(CFLAGS) lib\snolib\file.c

findunit.obj : lib\snolib\findunit.c
	$(CC) $(CFLAGS) lib\snolib\findunit.c

getenv.obj : lib\snolib\getenv.c
	$(CC) $(CFLAGS) lib\snolib\getenv.c

getstring.obj : lib\snolib\getstring.c
	$(CC) $(CFLAGS) lib\snolib\getstring.c

host.obj : lib\snolib\host.c
	$(CC) $(CFLAGS) lib\snolib\host.c

log.obj : lib\snolib\log.c
	$(CC) $(CFLAGS) lib\snolib\log.c

logic.obj : lib\snolib\logic.c
	$(CC) $(CFLAGS) lib\snolib\logic.c

rename.obj : lib\snolib\rename.c
	$(CC) $(CFLAGS) lib\snolib\rename.c

retstring.obj : lib\snolib\retstring.c
	$(CC) $(CFLAGS) lib\snolib\retstring.c

sin.obj : lib\snolib\sin.c
	$(CC) $(CFLAGS) lib\snolib\sin.c

sprintf.obj : lib\snolib\sprintf.c
	$(CC) $(CFLAGS) lib\snolib\sprintf.c

sqrt.obj : lib\snolib\sqrt.c
	$(CC) $(CFLAGS) lib\snolib\sqrt.c

sset.obj : lib\snolib\sset.c
	$(CC) $(CFLAGS) lib\snolib\sset.c

system.obj : lib\snolib\system.c
	$(CC) $(CFLAGS) lib\snolib\system.c

tan.obj : lib\snolib\tan.c
	$(CC) $(CFLAGS) lib\snolib\tan.c
