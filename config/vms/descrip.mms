# $Id$

# DESCRIP.MMS, hacked from Makefile2 for VMS "MMS"
# Build SNOBOL4 using DECC and MMS
#
# Tested on AXP OpenVMS 6.1 using DECC 4.0 and MMS 2.7 (DECSET 11.2-1)
#	(October 1996)

# Updated 9/3/97 (not tested)

SNOBOL4=isnobol4

BAL_C=[.lib]bal.c
CONVERT_C=[.lib.ansi]convert.c
DATE_C=[.lib]date.c
DUMP_C=[.lib]dump.c
DYNAMIC_C=[.lib.generic]dynamic.c
ENDEX_C=[.lib]endex.c
EXISTS_C=[.lib.generic]exists.c
EXPOPS_C=[.lib.generic]expops.c
HASH_C=[.lib]hash.c
# requires VAX/Ultrix connection product
INET_C=[.lib.bsd]inet.c
INIT_C=[.lib]init.c
INTSPC_C=[.lib]intspc.c
IO_C=[.lib]io.c
LEXCMP_C=[.lib]lexcmp.c
LOAD_C=[.lib.dummy]load.c
MSTIME_C=[.lib.vms]mstime.c
ORDVST_C=[.lib]ordvst.c
PAIR_C=[.lib]pair.c
PAT_C=[.lib]pat.c
PML_C=[.lib]pml.c
REALST_C=[.lib]realst.c
REPLACE_C=[.lib]replace.c
STREAM_C=[.lib]stream.c
STR_C=[.lib]str.c
TERM_C=[.lib.generic]term.c
TOP_C=[.lib]top.c
TREE_C=[.lib]tree.c
TTY_C=[.lib.vms]tty.c

# aux sources
BZERO_C=[.lib.auxil]bzero.c
BCOPY_C=[.lib.auxil]bcopy.c
GETOPT_C=[.lib.auxil]getopt.c
GETREDIRECT_C=[.lib.vms]getredirect.c
POPEN_C=[.lib.vms]popen.c

AUX_OBJ=bcopy.obj, bzero.obj, getopt.obj, getredirect.obj, popen.obj,

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
HOST_C=[.lib.snolib]host.c
LOG_C=[.lib.snolib]log.c
RENAME_C=[.lib.snolib]rename.c
RETSTRING_C=[.lib.snolib]retstring.c
SIN_C=[.lib.snolib]sin.c
SPRINTF_C=[.lib.snolib]sprintf.c
SQRT_C=[.lib.snolib]sqrt.c
SYS_C=[.lib.vms]sys.c
TAN_C=[.lib.snolib]tan.c

PML_OBJ=host.obj, sys.obj, exit.obj, execute.obj, sqrt.obj, \
	exp.obj, log.obj, chop.obj, sin.obj, cos.obj, tan.obj, \
	file.obj, delete.obj, rename.obj, findunit.obj, \
	getstring.obj, retstring.obj

CFLAGS=\
	/DECC/PREFIX_LIB=ALL/OPTIMIZE\
	/DEFINE=(ANSI_STRINGS,NO_OFF_T,TTY_READ_RAW,TTY_READ_COOKED,\
		ANY=XANY,DATE=XDATE,INIT=XINIT,LOAD=XLOAD,UNLOAD=XUNLOAD,\
		RPLACE=XRPLACE,TIME=XTIME,RAISE=XRAISE,DIV=XDIV,\
		SUBSTR=XSUBSTR,EXIT=XEXIT,SQRT=XSQRT,EXP=XEXP,LOG=XLOG,\
		SIN=XSIN,COS=XCOS,TAN=XTAN,RENAME=XRENAME)\
	/WARN=(DISABLE=IMPLICITFUNC) \
	/INCLUDE=(SYS$DISK:[],SYS$DISK:[.INCLUDE])

################

OBJS=	main.obj, $(SNOBOL4).obj, data.obj, data_init.obj, syn.obj, \
	bal.obj, convert.obj, date.obj, dynamic.obj, endex.obj, exists.obj, \
	expops.obj, hash.obj, init.obj, inet.obj, intspc.obj, io.obj, \
	lexcmp.obj, load.obj, mstime.obj, ordvst.obj, pair.obj, pat.obj, \
	pml.obj, realst.obj, replace.obj, str.obj, stream.obj, term.obj, \
	top.obj, tty.obj, tree.obj, version.obj, \
	$(AUX_OBJ) $(PML_OBJ)

snobol4.exe : $(OBJS)
	link /exec=snobol4.exe $(OBJS)

#################################################################
# lib files

bal.obj : $(BAL_C)
	$(CC) $(CFLAGS) $(BAL_C)

convert.obj : $(CONVERT_C)
	$(CC) $(CFLAGS) $(CONVERT_C)

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

str.obj : $(STR_C)
	$(CC) $(CFLAGS) $(STR_C)

stream.obj : $(STREAM_C)
	$(CC) $(CFLAGS) $(STREAM_C)

term.obj : $(TERM_C)
	$(CC) $(CFLAGS) $(TERM_C)

top.obj : $(TOP_C)
	$(CC) $(CFLAGS) $(TOP_C)

tree.obj : $(TREE_C)
	$(CC) $(CFLAGS) $(TREE_C)

tty.obj : $(TTY_C)
	$(CC) $(CFLAGS) $(TTY_C)

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

log.obj : $(LOG_C)
	$(CC) $(CFLAGS) $(LOG_C)

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

sys.obj : $(SYS_C)
	$(CC) $(CFLAGS) $(SYS_C)

tan.obj : $(TAN_C)
	$(CC) $(CFLAGS) $(TAN_C)

