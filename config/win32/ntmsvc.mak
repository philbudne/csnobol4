# $Id$

# nmake file for VC++ 5.0 on WinNT 4.0 by P. Budne 2/4/1998
# from batch file by David Feustel
# tested 10/2020 w/ Visual Studio Community Edition 2019 (v16.6.3)

################ settings

# 1, 2 or undefined for no internet support
# Winsock v1 available on Win95 and NT 3.5
# Winsock v2 available on Win98 and NT 4.0 (IPv6 available in WinXP?)
WINSOCK=2

# experimental Windows Pseudo Console support
WINPTY=1

# -O2 opt for speed
OPT=-O2

################ conditionals based on settings:

# dummy internet support
INET_OBJ=inet.obj
INET_SRC=$(SRCDIR)lib\dummy\inet.c

!ifdef WINSOCK
######## WINSOCK defined
!IF $(WINSOCK) == 1
#### winsock1
WINSOCK_DEF=-DHAVE_WINSOCK_H
INET_SRC=$(SRCDIR)lib\win32\inet.c
INET_OBJ=inet.obj
# wsock32 present on both Win95 and WinNT
INET_LIBS=wsock32.lib
!ELSEIF $(WINSOCK) == 2
#### winsock2
WINSOCK_DEF=-DHAVE_WINSOCK2_H
INET_SRC=$(SRCDIR)lib\bsd\inet6.c
INET_OBJ=inet6.obj
INET_LIBS=ws2_32.lib
!endif
# here with WINSOCK defined
INET_DEFS=$(WINSOCK_DEF) -DINET_IO
# auxillary objects required for both WS1 & WS2:
INET_OBJS=bindresvport.obj inetio_obj.obj
BUFIO=1
!ELSE
######## WINSOCK not defined
INET_SRC=$(SRCDIR)lib\dummy\inet.c
INET_OBJ=inet.obj
!ENDIF

!ifdef WINPTY
PTYIO_OBJ_SRC=$(SRCDIR)lib\win32\ptyio_obj.c
BUFIO=1
!else
PTYIO_OBJ_SRC=$(SRCDIR)lib\dummy\ptyio_obj.c
!endif

!ifdef MEMIO
MEMIO_OBJ=memio_obj.obj
BUFIO=1
!endif

!ifdef BUFIO
BUFIO_OBJ=bufio_obj.obj
!endif

################
CC=cl
LINK=link

COMMON_CFLAGS=-nologo -DHAVE_CONFIG_H
DL_CFLAGS=$(COMMON_CFLAGS)
CFLAGS=-c $(OPT) $(COMMON_CFLAGS) $(INET_DEFS) -I$(SRCDIR)config\win32 -I$(SRCDIR)include -I$(SRCDIR). $(DEFS) -DSNOBOL4

# disable switch/enum warning for isnobol4.c:
SNOBOL4_C_CFLAGS=/wd4715

OBJ=	$(BUFIO_OBJ) $(INET_OBJ) $(INET_OBJS) $(MEMIO_OBJ) \
	atan.obj bal.obj break.obj chop.obj cos.obj data.obj \
	data_init.obj date.obj delete.obj dump.obj \
	dynamic.obj endex.obj environ.obj execute.obj \
	exists.obj exit.obj exp.obj expops.obj file.obj \
	findunit.obj getline.obj getopt.obj getstring.obj \
	handle.obj hash.obj host.obj init.obj intspc.obj \
	io.obj isnobol4.obj lexcmp.obj load.obj loadx.obj \
	log.obj main.obj mstime.obj newer.obj ord.obj ordvst.obj \
	osopen.obj pair.obj pat.obj pml.obj ptyio_obj.obj \
	realst.obj rename.obj replace.obj retstring.obj \
	sin.obj spcint.obj spreal.obj sqrt.obj sset.obj \
	stdio_obj.obj str.obj stream.obj syn.obj sys.obj \
	tan.obj term.obj top.obj tree.obj tty.obj


all:	cpuid.exe snobol4.exe build_modules docs

cpuid.exe: cpuid.c
	$(CC) -c cpuid.c
	$(LINK) /out:cpuid.exe cpuid.obj

MANIFEST_RC=$(SRCDIR)config\win32\manifest.rc
MANIFEST_RES=manifest.res

snobol4.exe : always $(OBJ) $(MANIFEST_RES)
	$(LINK) /out:snobol4.exe $(OBJ) $(MANIFEST_RES) $(INET_LIBS)

$(MANIFEST_RES) : $(MANIFEST_RC)
	rc /r /fo $(MANIFEST_RES) $(MANIFEST_RC)

# kill leftovers from cygwin builds!!!
always:
	@if EXIST config.h erase config.h
	@if EXIST config.sno erase config.sno

################ DLL (not tested!)

DLLDIR=dllobj
DLLNAME=snobol4.dll
DLLLIB=snobol4.lib

MAKEFILE=$(MAKEDIR)\config\win32\ntmsvc.mak

# NOTE -DDLL not defined!! That's for loadable modules!!
dll $(DLLDIR)\$(DLLNAME) $(DLLDIR)\$(DLLLIB): always
	if NOT EXIST $(DLLDIR) mkdir $(DLLDIR)
	cd $(DLLDIR)
	nmake -f $(MAKEFILE) DEFS=-DSHARED MEMIO=1 SRCDIR=..\ dll2 dlltests

################
# invoked by above, in DLLDIR, with tweaked variables

# target NOT named snobol4.dll, 'cause someone might try "make snobol4.dll"
# at top level, which would pick up the wrong .obj files.
# tried adding MANIFEST_RES to .dll, doesn't seem to work.
dll2:	$(OBJ)
	$(LINK) /DLL /out:$(DLLNAME) $(OBJ) $(INET_LIBS)

# DLL test programs. Seem to need manifest.res file here:
# ssnobol4.exe is the regular main program using snobol4.dll
# tlib.exe is a test invoking interpreter with tiny (hello world) program
dlltests: dll2 $(MANIFEST_RES)
	$(CC) /Fe:ssnobol4.exe -I..\include ..\ssnobol4.c $(MANIFEST_RES) $(DLLLIB)
	$(CC) /Fe:tlib.exe -I..\include ..\tlib.c $(MANIFEST_RES) $(DLLLIB)

################

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

stdio_obj.obj : $(SRCDIR)lib\stdio_obj.c
	$(CC) $(CFLAGS) $(SRCDIR)lib\stdio_obj.c

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

getline.obj : $(SRCDIR)lib\auxil\getline.c
	$(CC) $(CFLAGS) $(SRCDIR)lib\auxil\getline.c

getopt.obj : $(SRCDIR)lib\auxil\getopt.c
	$(CC) $(CFLAGS) $(SRCDIR)lib\auxil\getopt.c

bindresvport.obj : $(SRCDIR)lib\auxil\bindresvport.c
	$(CC) $(CFLAGS) $(SRCDIR)lib\auxil\bindresvport.c

################ generic

dynamic.obj : $(SRCDIR)lib\generic\dynamic.c
	$(CC) $(CFLAGS) $(SRCDIR)lib\generic\dynamic.c

expops.obj : $(SRCDIR)lib\generic\expops.c
	$(CC) $(CFLAGS) $(SRCDIR)lib\generic\expops.c

intspc.obj : $(SRCDIR)lib\generic\intspc.c
	$(CC) $(CFLAGS) $(SRCDIR)lib\generic\intspc.c

newer.obj : $(SRCDIR)lib\generic\newer.c
	$(CC) $(CFLAGS) $(SRCDIR)lib\generic\newer.c

################ auxil

bufio_obj.obj : $(SRCDIR)lib\auxil\bufio_obj.c
	$(CC) $(CFLAGS) $(SRCDIR)lib\auxil\bufio_obj.c

memio_obj.obj : $(SRCDIR)lib\auxil\memio_obj.c
	$(CC) $(CFLAGS) $(SRCDIR)lib\auxil\memio_obj.c

################ win32!

execute.obj : $(SRCDIR)lib\win32\execute.c
	$(CC) $(CFLAGS) $(SRCDIR)lib\win32\execute.c

# or dummy, or bsd/inet6.c!!
$(INET_OBJ) : $(INET_SRC)
	$(CC) $(CFLAGS) $(INET_SRC)

inetio_obj.obj : $(SRCDIR)lib\win32\inetio_obj.c
	$(CC) $(CFLAGS) $(SRCDIR)lib\win32\inetio_obj.c

load.obj : $(SRCDIR)lib\win32\load.c
	$(CC) $(CFLAGS) $(SRCDIR)lib\win32\load.c

mstime.obj : $(SRCDIR)lib\win32\mstime.c
	$(CC) $(CFLAGS) $(SRCDIR)lib\win32\mstime.c

osopen.obj : $(SRCDIR)lib\win32\osopen.c
	$(CC) $(CFLAGS) $(SRCDIR)lib\win32\osopen.c

ptyio_obj.obj : $(PTYIO_OBJ_SRC)
	$(CC) $(CFLAGS) $(PTYIO_OBJ_SRC)

sys.obj : $(SRCDIR)lib\win32\sys.c
	$(CC) $(CFLAGS) $(SRCDIR)lib\win32\sys.c

term.obj : $(SRCDIR)lib\win32\term.c
	$(CC) $(CFLAGS) $(SRCDIR)lib\win32\term.c

tty.obj : $(SRCDIR)lib\win32\tty.c
	$(CC) $(CFLAGS) $(SRCDIR)lib\win32\tty.c

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

# had depended on $(MAKEFILE) -- causing grief in dll make
host.obj : $(SRCDIR)lib\snolib\host.c $(SRCDIR)config\win32\config.h
	$(CC) $(CFLAGS) $(SRCDIR)lib\snolib\host.c -DCC=\"$(CC)\" -DCOPT=\"$(OPT)\" -DSO_LD=\"$(LINK)\" -DDL_LD=\"$(LINK)\" -DDL_CFLAGS=\""$(DL_CFLAGS)\""

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

docs:
	config\win32\format.bat

# cannot be install due to INSTALL doc file
runinstall:
	pkg\win32\install.bat

tar:
	pkg\win32\maketar.bat

clean:
	if EXIST snobol5.exe config\win32\modules.bat clean
	erase /f/q/s *.obj *.exe *.res
	erase /f/q/s doc\*.html
	erase /f/q/s snolib4.lib snobol4.exp
	if EXIST $(DLLDIR) erase /f/q/s $(DLLDIR)\*
	if EXIST $(DLLDIR) rmdir $(DLLDIR)
