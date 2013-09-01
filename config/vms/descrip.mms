# $Id$

# DESCRIP.MMS, hacked from Makefile2 for VMS "MMS"
# Build SNOBOL4 using DECC or VAXC and MMS
#
# **** see INSTALL file for usage ****

# *** NOTE *** This file does NOT contain full dependancy information!!!
# It's always best to remove all .OBJ files and rebuild from scratch!!!

.ifdef VAX_C
# Tested on VAX OpenVMS 6.1 using VAXC 3.1 (June 2003)
# need to suppress warnings (about LIB$ and SYS$ routines)
# otherwise CC returns status that makes MMS unhappy
CCFLAGS=/OPTIMIZE/WARNINGS=NOWARNINGS
AUX_OBJ=isnan.obj, finite.obj, \
	bcopy.obj, bzero.obj, getopt.obj, popen.obj, unlink.obj, 
CCDEFS=,NEED_BCOPY,NEED_BZERO,NEED_POPEN,NEED_OFF_T

# need explicit C runtime library
#CLIB=+SYS$SHARE:VAXCRTL/SHARE
CLIB=+SYS$LIBRARY:DECCRTL/LIB

# alternate library routines
MSTIME_C=[.lib.vms]mstime.c

# need funny includes, bindresvport
UCXDEFS=,OLD_UCX_INCLUDES,NEED_BINDRESVPORT
UCXOBJ=inet.obj, bindresvport.obj
UCXLIB=+SYS$LIBRARY:UCX$IPC/LIB

.else
.ifdef DECC4
# Tested under AXP OpenVMS 6.2 using DECC 4.0 (July 2003)
CCFLAGS=/PREFIX_LIB=ALL/WARN=(DISABLE=IMPLICITFUNC)/OPTIMIZE

AUX_OBJ=bcopy.obj, bzero.obj, popen.obj, unlink.obj, 
CCDEFS=,NEED_BCOPY,NEED_BZERO,NEED_POPEN,SOCKLEN_INT,HAVE_UNIXIO_H
# no explicit CRT library needed

# alternate library routines
MSTIME_C=[.lib.vms]mstime.c

# don't need odd includes
UCXDEFS=,NEED_BINDRESVPORT
UCXOBJ=inet.obj, bindresvport.obj
UCXLIB=+SYS$LIBRARY:UCX$IPC/LIB

.else

# ** DEFAULT**
# Tested under:
#	Compaq C V6.5-001 on OpenVMS Alpha V8.2 (December 2005)
#	HP C S7.1-013 on OpenVMS IA64 V8.2-1 (December 2005)

# downright civilized!!

CCFLAGS=/OPTIMIZE
CCDEFS=,HAVE_STRINGS_H,HAVE_STDLIB_H,HAVE_UNISTD_H
# no explicit CRT library needed

# alternate (normal!) library routines
MSTIME_C=[.lib.posix]mstime.c

UCXOBJ=inet6.obj, bindresvport.obj
UCXDEFS=,NEED_BINDRESVPORT_SA
.endif
.endif

.ifdef NO_TCP
# C compiler flags, if any
INET_C=[.lib.dummy]inet.c
INETDEFS=
INETOBJ=inet.obj

.else

# XXX ifdefs here for different TCP/IP packages?

################
# use DEC TCP/IP Connection services for OpenVMS
#	(formerly VAX/Ultrix connection product)

# C compiler flags, if any
INET_C=[.lib.bsd]inet.c
INETDEFS=$(UCXDEFS)
INETOBJ=$(UCXOBJ)
INETLIB=$(UCXLIB)
.endif

LIBS=$(INETLIB) $(CLIB)

SNOBOL4=isnobol4

BAL_C=[.lib]bal.c
BREAK_C=[.lib]break.c
DATE_C=[.lib]date.c
DUMP_C=[.lib]dump.c
DYNAMIC_C=[.lib.generic]dynamic.c
ENDEX_C=[.lib]endex.c
EXISTS_C=[.lib.vms]exists.c
EXPOPS_C=[.lib.generic]expops.c
HASH_C=[.lib]hash.c
INET6_C=[.lib.bsd]inet6.c
INIT_C=[.lib]init.c
INTSPC_C=[.lib.generic]intspc.c
IO_C=[.lib]io.c
LEXCMP_C=[.lib]lexcmp.c
LOAD_C=[.lib.vms]load.c
ORDVST_C=[.lib]ordvst.c
OSOPEN_C=[.lib.vms]osopen.c
PAIR_C=[.lib]pair.c
PAT_C=[.lib]pat.c
PML_C=[.lib]pml.c
POPEN_C=[.lib.vms]popen.c
REALST_C=[.lib]realst.c
REPLACE_C=[.lib]replace.c
SPCINT_C=[.lib.ansi]spcint.c
SPREAL_C=[.lib.ansi]spreal.c
STREAM_C=[.lib]stream.c
STR_C=[.lib]str.c
TERM_C=[.lib.vms]term.c
TOP_C=[.lib]top.c
TREE_C=[.lib]tree.c
TTY_C=[.lib.vms]tty.c

# aux sources
BZERO_C=[.lib.auxil]bzero.c
BCOPY_C=[.lib.auxil]bcopy.c
GETOPT_C=[.lib.auxil]getopt.c
GETREDIRECT_C=[.lib.vms]getredirect.c

# snolib sources
CHOP_C=[.lib.snolib]chop.c
COS_C=[.lib.snolib]cos.c
DELETE_C=[.lib.snolib]delete.c
EXECUTE_C=[.lib.generic]execute.c
EXIT_C=[.lib.snolib]exit.c
EXP_C=[.lib.snolib]exp.c
FILE_C=[.lib.snolib]file.c
FINDUNIT_C=[.lib.snolib]findunit.c
FORK_C=[.lib.snolib]fork.c
GETSTRING_C=[.lib.snolib]getstring.c
HANDLE_C=[.lib.snolib]handle.c
HOST_C=[.lib.snolib]host.c
LOG_C=[.lib.snolib]log.c
LOGIC_C=[.lib.snolib]logic.c
NDBM_C=[.lib.snolib]ndbm.c
ORD_C=[.lib.snolib]ord.c
RENAME_C=[.lib.snolib]rename.c
RETSTRING_C=[.lib.snolib]retstring.c
SIN_C=[.lib.snolib]sin.c
SPRINTF_C=[.lib.snolib]sprintf.c
SQRT_C=[.lib.snolib]sqrt.c
SSET_C=[.lib.snolib]sset.c
SYS_C=[.lib.vms]sys.c
TAN_C=[.lib.snolib]tan.c

PML_OBJ=chop.obj, cos.obj, delete.obj, execute.obj, exit.obj, exp.obj, \
	file.obj, findunit.obj, getstring.obj, handle.obj, host.obj, \
	log.obj, logic.obj, ndbm.obj, ord.obj, rename.obj, retstring.obj, \
	sin.obj, sqrt.obj, sset.obj, sys.obj, tan.obj

# .ifdef this if you want to use something else!
NDBM_DEFS=,HAVE_SDBM_H
NDBM_INCLUDE=,SYS$DISK:[.LIB.SDBM]
NDBM_OBJ=,sdbm.obj, sdbm_hash.obj, sdbm_pair.obj

CFLAGS=	$(CCFLAGS) /DEFINE=(HAVE_CONFIG_H$(CCDEFS)$(INETDEFS)$(NDBM_DEFS)) \
	/INCLUDE=(SYS$DISK:[.CONFIG.VMS],SYS$DISK:[],SYS$DISK:[.INCLUDE]$(NDBM_INCLUDE))

################

OBJS=	main.obj, $(SNOBOL4).obj, data.obj, data_init.obj, syn.obj, \
	bal.obj, break.obj, date.obj, dynamic.obj, endex.obj, exists.obj, \
	expops.obj, hash.obj, init.obj, $(INETOBJ), intspc.obj, \
	io.obj, lexcmp.obj, load.obj, mstime.obj, ordvst.obj, \
	osopen.obj, pair.obj, pat.obj, pml.obj, realst.obj, \
	replace.obj, spcint.obj, spreal.obj, str.obj, stream.obj, \
	term.obj, top.obj, tty.obj, tree.obj, \
	getredirect.obj, $(AUX_OBJ) $(PML_OBJ) $(NDBM_OBJ)

snobol4.exe : $(OBJS)
	link /exec=snobol4.exe $(OBJS) $(LIBS)

#################################################################
# lib files

bal.obj : $(BAL_C)
	$(CC) $(CFLAGS) $(BAL_C)

break.obj : $(BREAK_C)
	$(CC) $(CFLAGS) $(BREAK_C)

date.obj : $(DATE_C)
	$(CC) $(CFLAGS) $(DATE_C)

dump.obj : $(DUMP_C)
	$(CC) $(CFLAGS) $(DUMP_C)

dynamic.obj : $(DYNAMIC_C)
	$(CC) $(CFLAGS) $(DYNAMIC_C)

endex.obj : $(ENDEX_C)
	$(CC) $(CFLAGS) $(ENDEX_C)

exists.obj : $(EXISTS_C)
	$(CC) $(CFLAGS) $(EXISTS_C)

expops.obj : $(EXPOPS_C)
	$(CC) $(CFLAGS) $(EXPOPS_C)

hash.obj : $(HASH_C)
	$(CC) $(CFLAGS) $(HASH_C)

inet.obj : $(INET_C)
	$(CC) $(CFLAGS) $(INET_C)

inet6.obj : $(INET6_C)
	$(CC) $(CFLAGS) $(INET6_C)

init.obj : $(INIT_C)
	$(CC) $(CFLAGS) $(INIT_C)

intspc.obj : $(INTSPC_C)
	$(CC) $(CFLAGS) $(INTSPC_C)

io.obj : $(IO_C)
	$(CC) $(CFLAGS) $(IO_C)

lexcmp.obj : $(LEXCMP_C)
	$(CC) $(CFLAGS) $(LEXCMP_C)

load.obj : $(LOAD_C)
	$(CC) $(CFLAGS) $(LOAD_C)

mstime.obj : $(MSTIME_C)
	$(CC) $(CFLAGS) $(MSTIME_C)

ordvst.obj : $(ORDVST_C)
	$(CC) $(CFLAGS) $(ORDVST_C)

osopen.obj : $(OSOPEN_C)
	$(CC) $(CFLAGS) $(OSOPEN_C)

pair.obj : $(PAIR_C)
	$(CC) $(CFLAGS) $(PAIR_C)

pat.obj : $(PAT_C)
	$(CC) $(CFLAGS) $(PAT_C)

pml.obj : $(PML_C)
	$(CC) $(CFLAGS) $(PML_C)

realst.obj : $(REALST_C)
	$(CC) $(CFLAGS) $(REALST_C)

replace.obj : $(REPLACE_C)
	$(CC) $(CFLAGS) $(REPLACE_C)

spcint.obj : $(SPCINT_C)
	$(CC) $(CFLAGS) $(SPCINT_C)

spreal.obj : $(SPREAL_C)
	$(CC) $(CFLAGS) $(SPREAL_C)

str.obj : $(STR_C)
	$(CC) $(CFLAGS) $(STR_C)

stream.obj : $(STREAM_C)
	$(CC) $(CFLAGS) $(STREAM_C)

top.obj : $(TOP_C)
	$(CC) $(CFLAGS) $(TOP_C)

tree.obj : $(TREE_C)
	$(CC) $(CFLAGS) $(TREE_C)

################
# system dependant

term.obj : $(TERM_C)
	$(CC) $(CFLAGS) $(TERM_C)

tty.obj : $(TTY_C)
	$(CC) $(CFLAGS) $(TTY_C)

popen.obj : $(POPEN_C)
	$(CC) $(CFLAGS) $(POPEN_C)

#################
# porting aids

bzero.obj : $(BZERO_C)
	$(CC) $(CFLAGS) $(BZERO_C)

bcopy.obj : $(BCOPY_C)
	$(CC) $(CFLAGS) $(BCOPY_C)

getopt.obj : $(GETOPT_C)
	$(CC) $(CFLAGS) $(GETOPT_C)

getredirect.obj : $(GETREDIRECT_C)
	$(CC) $(CFLAGS) $(GETREDIRECT_C)

################

chop.obj : $(CHOP_C)
	$(CC) $(CFLAGS) $(CHOP_C)

cos.obj : $(COS_C)
	$(CC) $(CFLAGS) $(COS_C)

delete.obj : $(DELETE_C)
	$(CC) $(CFLAGS) $(DELETE_C)

environ.obj : $(ENVIRON_C)
	$(CC) $(CFLAGS) $(ENVIRON_C)

execute.obj : $(EXECUTE_C)
	$(CC) $(CFLAGS) $(EXECUTE_C)

exit.obj : $(EXIT_C)
	$(CC) $(CFLAGS) $(EXIT_C)

exp.obj : $(EXP_C)
	$(CC) $(CFLAGS) $(EXP_C)

file.obj : $(FILE_C)
	$(CC) $(CFLAGS) $(FILE_C)

findunit.obj : $(FINDUNIT_C)
	$(CC) $(CFLAGS) $(FINDUNIT_C)

fork.obj : $(FORK_C)
	$(CC) $(CFLAGS) $(FORK_C)

getstring.obj : $(GETSTRING_C)
	$(CC) $(CFLAGS) $(GETSTRING_C)

host.obj : $(HOST_C)
	$(CC) $(CFLAGS) $(HOST_C)

handle.obj : $(HANDLE_C)
	$(CC) $(CFLAGS) $(HANDLE_C)

log.obj : $(LOG_C)
	$(CC) $(CFLAGS) $(LOG_C)

logic.obj : $(LOGIC_C)
	$(CC) $(CFLAGS) $(LOGIC_C)

ndbm.obj : $(NDBM_C)
	$(CC) $(CFLAGS) $(NDBM_C)

ord.obj : $(ORD_C)
	$(CC) $(CFLAGS) $(ORD_C)

rename.obj : $(RENAME_C)
	$(CC) $(CFLAGS) $(RENAME_C)

retstring.obj : $(RETSTRING_C)
	$(CC) $(CFLAGS) $(RETSTRING_C)

sin.obj : $(SIN_C)
	$(CC) $(CFLAGS) $(SIN_C)

sprintf.obj : $(SPRINTF_C)
	$(CC) $(CFLAGS) $(SPRINTF_C)

sqrt.obj : $(SQRT_C)
	$(CC) $(CFLAGS) $(SQRT_C)

sset.obj : $(SSET_C)
	$(CC) $(CFLAGS) $(SSET_C)

sys.obj : $(SYS_C)
	$(CC) $(CFLAGS) $(SYS_C)

tan.obj : $(TAN_C)
	$(CC) $(CFLAGS) $(TAN_C)

unlink.obj : [.lib.vms]unlink.c
	$(CC) $(CFLAGS) [.lib.vms]unlink.c

# vax:
isnan.obj : [.lib.dummy]isnan.c
	$(CC) $(CFLAGS) [.lib.dummy]isnan.c

# vax:
finite.obj : [.lib.dummy]finite.c
	$(CC) $(CFLAGS) [.lib.dummy]finite.c

bindresvport.obj : [.lib.auxil]bindresvport.c
	$(CC) $(CFLAGS) [.lib.auxil]bindresvport.c

################
# SDBM (NDBM replacement)

sdbm.obj : [.lib.sdbm]sdbm.c
	$(CC) $(CFLAGS) [.lib.sdbm]sdbm.c

sdbm_hash.obj : [.lib.sdbm]sdbm_hash.c
	$(CC) $(CFLAGS) [.lib.sdbm]sdbm_hash.c

sdbm_pair.obj : [.lib.sdbm]sdbm_pair.c
	$(CC) $(CFLAGS) [.lib.sdbm]sdbm_pair.c
