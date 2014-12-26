# $Id$

# nmake file for VC++ 5.0 on WinNT 4.0 by P. Budne 2/4/1998
# from batch file by David Feustel

# tested 11/2005 w/ Visual Studio .NET 2003 (VC71)
# tested  5/2010 w/ Visual C++ Express 2010

CC=cl
LINK=link

# -O2 opt for speed
OPT=-O2

# can also use msdos version (less friendly in multitasking env)
TTY_C=$(SRCDIR)lib\win32\tty.c

# crocks for winsock I/O on Win9x
INET_DEFS=-DINET_IO
# wsock32 present on both Win95 and WinNT
INET_LIBS=wsock32.lib

COMMON_CFLAGS=-nologo -DHAVE_CONFIG_H
DL_CFLAGS=$(COMMON_CFLAGS)
CFLAGS=-c $(OPT) $(COMMON_CFLAGS) $(INET_DEFS) \
	-I$(SRCDIR)config\win32 -I$(SRCDIR)include -I$(SRCDIR).

SNOBOL4_C_CFLAGS=/wd4715

OBJ=	isnobol4.obj data.obj data_init.obj main.obj syn.obj \
	bal.obj break.obj date.obj dump.obj endex.obj hash.obj \
	intspc.obj io.obj lexcmp.obj ordvst.obj pair.obj \
	pat.obj pml.obj realst.obj replace.obj str.obj stream.obj \
	top.obj tree.obj dynamic.obj expops.obj \
	getopt.obj init.obj load.obj loadx.obj mstime.obj \
	atan.obj chop.obj cos.obj \
	delete.obj environ.obj exit.obj file.obj getstring.obj handle.obj \
	host.obj log.obj ord.obj rename.obj retstring.obj \
	sin.obj spcint.obj spreal.obj sqrt.obj sset.obj tan.obj \
	osopen.obj sleep.obj sys.obj tty.obj inet.obj bindresvport.obj \
	execute.obj exists.obj term.obj findunit.obj exp.obj

all:	cpuid.exe snobol4.exe build_modules

cpuid.exe: cpuid.c
	$(CC) -c cpuid.c
	$(LINK) /out:cpuid.exe cpuid.obj

snobol4.exe : always $(OBJ)
	$(LINK) /out:snobol4.exe $(OBJ) $(INET_LIBS)

# kill leftovers from cygwin builds!!!
always:
	if EXIST config.h erase config.h
	if EXIST config.sno erase config.sno

data.obj : $(SRCDIR)data.c
	$(CC) $(CFLAGS) $(SRCDIR)data.c

data_init.obj : $(SRCDIR)data_init.c
	$(CC) $(CFLAGS) $(SRCDIR)data_init.c

isnobol4.obj : $(SRCDIR)isnobol4.c
	$(CC) $(CFLAGS) $(SNOBOL4_C_CFLAGS) $(SRCDIR)isnobol4.c

main.obj : $(SRCDIR)main.c
	$(CC) $(CFLAGS) $(SRCDIR)main.c

syn.obj : $(SRCDIR)syn.c
	$(CC) $(CFLAGS) $(SRCDIR)syn.c

################ common

bal.obj : $(SRCDIR)lib\bal.c
	$(CC) $(CFLAGS) $(SRCDIR)lib\bal.c

break.obj : $(SRCDIR)lib\break.c
	$(CC) $(CFLAGS) $(SRCDIR)lib\break.c

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

loadx.obj : $(SRCDIR)lib\loadx.c
	$(CC) $(CFLAGS) $(SRCDIR)lib\loadx.c

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

osopen.obj : $(SRCDIR)lib\win32\osopen.c
	$(CC) $(CFLAGS) $(SRCDIR)lib\win32\osopen.c

sleep.obj : $(SRCDIR)lib\win32\sleep.c
	$(CC) $(CFLAGS) $(SRCDIR)lib\win32\sleep.c

sys.obj : $(SRCDIR)lib\win32\sys.c
	$(CC) $(CFLAGS) $(SRCDIR)lib\win32\sys.c

term.obj : $(SRCDIR)lib\win32\term.c
	$(CC) $(CFLAGS) $(SRCDIR)lib\win32\term.c

tty.obj : $(TTY_C)
	$(CC) $(CFLAGS) $(TTY_C)

exists.obj : $(SRCDIR)lib\win32\exists.c
	$(CC) $(CFLAGS) $(SRCDIR)lib\win32\exists.c

################ snolib

atan.obj : $(SRCDIR)lib\snolib\atan.c
	$(CC) $(CFLAGS) $(SRCDIR)lib\snolib\atan.c

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

handle.obj : $(SRCDIR)lib\snolib\handle.c
	$(CC) $(CFLAGS) $(SRCDIR)lib\snolib\handle.c

host.obj : $(SRCDIR)lib\snolib\host.c config\win32\ntmsvc.mak config\win32\config.h
	$(CC) $(CFLAGS) $(SRCDIR)lib\snolib\host.c -DCC=\"$(CC)\" -DCOPT=\"$(OPT)\" -DSO_LD=\"$(LINK)\" -DDL_LD=\"$(LINK)\" -DDL_CFLAGS=\"$(DL_CFLAGS)\"

log.obj : $(SRCDIR)lib\snolib\log.c
	$(CC) $(CFLAGS) $(SRCDIR)lib\snolib\log.c

ord.obj : $(SRCDIR)lib\snolib\ord.c
	$(CC) $(CFLAGS) $(SRCDIR)lib\snolib\ord.c

random.obj : $(SRCDIR)lib\snolib\random.c
	$(CC) $(CFLAGS) $(SRCDIR)lib\snolib\random.c

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

time.obj : $(SRCDIR)lib\snolib\time.c
	$(CC) $(CFLAGS) $(SRCDIR)lib\snolib\time.c

################################################################

build_modules:
	config\win32\modules.bat

install:
	pkg\win32\install.bat
