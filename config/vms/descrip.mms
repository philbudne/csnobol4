# $Id$

# DESCRIP.MMS, hacked from Makefile2 for VMS "MMS"
# Build SNOBOL4 using DECC and MMS
#
# Tested on AXP OpenVMS 6.1 using DECC 4.0 and MMS 2.7 (DECSET 11.2-1)

SNOBOL4=isnobol4

BAL_C=[.lib]bal.c
CONVERT_C=[.lib.ansi]convert.c
DATE_C=[.lib]date.c
DUMP_C=[.lib]dump.c
DYNAMIC_C=[.lib.generic]dynamic.c
ENDEX_C=[.lib]endex.c
EXP_C=[.lib.generic]exp.c
HASH_C=[.lib]hash.c
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
TERM_C=[.lib.vms]term.c
TOP_C=[.lib]top.c
TREE_C=[.lib]tree.c

# aux sources
BZERO_C=[.lib.auxil]bzero.c
BCOPY_C=[.lib.auxil]bcopy.c
GETOPT_C=[.lib.auxil]getopt.c
GETREDIRECT_C=[.lib.vms]getredirect.c

AUX_OBJ=bcopy.obj, bzero.obj, getopt.obj, getredirect.obj

CFLAGS=\
	/DECC/PREFIX_LIB=ALL/OPTIMIZE\
	/DEFINE=(ANSI_STRINGS,NO_POPEN,NO_OFF_T,\
		DATE=XDATE,INIT=XINIT,LOAD=XLOAD,UNLOAD=XUNLOAD,\
		RPLACE=XRPLACE,TIME=XTIME,RAISE=XRAISE,DIV=XDIV,\
		SUBSTR=XSUBSTR) \
	/WARN=(DISABLE=IMPLICITFUNC) \
	/INCLUDE=(SYS$DISK:[],SYS$DISK:[.INCLUDE]) \

################

OBJS=	main.obj, $(SNOBOL4).obj, data.obj, data_init.obj, syn.obj, \
	bal.obj, convert.obj, date.obj, dynamic.obj, endex.obj, \
	exp.obj, hash.obj, init.obj, intspc.obj, io.obj, lexcmp.obj, \
	load.obj, mstime.obj, ordvst.obj, pair.obj, pat.obj, pml.obj, \
	realst.obj, replace.obj, str.obj, stream.obj, term.obj, top.obj, \
	tree.obj, version.obj, $(CONFIG_OBJ) $(AUX_OBJ) $(PML_OBJ)

xsnobol4.exe : $(OBJS)
	link /exec=xsnobol4.exe $(OBJS)

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

exp.obj : $(EXP_C)
	$(CC) $(CFLAGS) $(EXP_C)

hash.obj : $(HASH_C)
	$(CC) $(CFLAGS) $(HASH_C)

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
