# $Id$
changequote([,])dnl
define([CMT],[dnl])dnl
CMT([################################################################])
CMT([establish m4 macros to collect various options])
CMT()
CMT([extra C compiler cpp flags for all c files])
define([ADD_CPPFLAGS],[define([_CPPFLAGS],_CPPFLAGS $1)dnl])dnl
define([_CPPFLAGS],)dnl
CMT()
CMT([extra C compiler optimize/debug flags for all c files])
define([ADD_OPT],[define([_OPT],_OPT $1)dnl])dnl
define([_OPT],)dnl
CMT()
CMT([extra source files for make depend])
define([ADD_SRCS],[define([_SRCS],_SRCS $1)dnl])dnl
define([_SRCS],)dnl
CMT()
CMT([extra object files for final link])
define([ADD_OBJS],[define([_OBJS],_OBJS $1)dnl])dnl
define([_OBJS],)dnl
CMT()
CMT([extra C compiler flags for final link])
define([ADD_LDFLAGS],[define([_LDFLAGS],_LDFLAGS $1)dnl])dnl
define([_LDFLAGS],)dnl

################################################################
# defaults (may be overridden in config.h)

OPT=-O

CCM=./cc-M

# for pow(3)
MATHLIB=-lm

# either snobol4 or isnobol4
SNOBOL4=isnobol4
#SNOBOL4=snobol4

# library random accessifier
RANLIB=ranlib

# filename for external function library
SNOLIB_A=snolib.a

# directory name for default SNOLIB (used by -INCLUDE and LOAD())
SNOLIB_DIR=/usr/local/lib/snobol4
########
# default lib source files

BAL_C=$(SRCDIR)lib/bal.c
CONVERT_C=$(SRCDIR)lib/ansi/convert.c
DATE_C=$(SRCDIR)lib/date.c
DUMP_C=$(SRCDIR)lib/dump.c
DYNAMIC_C=$(SRCDIR)lib/bsd/dynamic.c
ENDEX_C=$(SRCDIR)lib/endex.c
EXPOPS_C=$(SRCDIR)lib/generic/expops.c
HASH_C=$(SRCDIR)lib/hash.c
INIT_C=$(SRCDIR)lib/init.c
INTSPC_C=$(SRCDIR)lib/intspc.c
IO_C=$(SRCDIR)lib/io.c
LEXCMP_C=$(SRCDIR)lib/lexcmp.c
LOAD_C=$(SRCDIR)lib/bsd/load.c
MSTIME_C=$(SRCDIR)lib/bsd/mstime.c
ORDVST_C=$(SRCDIR)lib/ordvst.c
PAIR_C=$(SRCDIR)lib/pair.c
PAT_C=$(SRCDIR)lib/pat.c
PML_C=$(SRCDIR)lib/pml.c
REALST_C=$(SRCDIR)lib/realst.c
REPLACE_C=$(SRCDIR)lib/replace.c
STREAM_C=$(SRCDIR)lib/stream.c
STR_C=$(SRCDIR)lib/str.c
TERM_C=$(SRCDIR)lib/posix/term.c
TOP_C=$(SRCDIR)lib/top.c
TREE_C=$(SRCDIR)lib/tree.c

# aux sources
BZERO_C=$(SRCDIR)lib/auxil/bzero.c
BCOPY_C=$(SRCDIR)lib/auxil/bcopy.c
GETOPT_C=$(SRCDIR)lib/auxil/getopt.c
ISNAN_C=$(SRCDIR)lib/dummy/isnan.c

# snolib sources
CHOP_C=$(SRCDIR)lib/snolib/chop.c
COS_C=$(SRCDIR)lib/snolib/cos.c
DELETE_C=$(SRCDIR)lib/snolib/delete.c
ENVIRON_C=$(SRCDIR)lib/snolib/environ.c
EXIT_C=$(SRCDIR)lib/snolib/exit.c
EXP_C=$(SRCDIR)lib/snolib/exp.c
FILE_C=$(SRCDIR)lib/snolib/file.c
FORK_C=$(SRCDIR)lib/snolib/fork.c
GETSTRING_C=$(SRCDIR)lib/snolib/getstring.c
HOST_C=$(SRCDIR)lib/snolib/host.c
LOG_C=$(SRCDIR)lib/snolib/log.c
RENAME_C=$(SRCDIR)lib/snolib/rename.c
RETSTRING_C=$(SRCDIR)lib/snolib/retstring.c
SIN_C=$(SRCDIR)lib/snolib/sin.c
SPRINTF_C=$(SRCDIR)lib/snolib/sprintf.c
SQRT_C=$(SRCDIR)lib/snolib/sqrt.c
TAN_C=$(SRCDIR)lib/snolib/tan.c

# private copy of CFLAGS for data_init.o; here so it can be overridden
# (ie; to just $(MYCPPFLAGS)) by config.m4 during debug (optimizing it
# is painful but worthwhile for production).

DATA_INIT_CFLAGS=$(CFLAGS)

# end of defaults
################################################################
CMT()
CMT([establish base values:])
ADD_OPT([$(OPT)])
ADD_LDFLAGS([$(MATHLIB)])
CMT()
# config.m4:
include(config.m4)

# end of local config
################################################################

# after local config

# NOTE: NOT named CPPFLAGS; some versions of make include CPPFLAGS in cc cmd
MYCPPFLAGS=-I./[include] -I. _CPPFLAGS

COPT=[]_OPT

LDFLAGS=[]_LDFLAGS

SNOLIB_DEFINES=-DSNOLIB_DIR='"'$(SNOLIB_DIR)'"' -DSNOLIB_A='"'$(SNOLIB_A)'"'

################
# compiler flags

CFLAGS=$(COPT) $(MYCPPFLAGS)

################
# snobol commands

# bootstrapped using Catspaw SPARC SPITBOL
#SNO=spitbol -i512k -b
#SMALL_SNO=spitbol -b
SNO=snobol4 -d512k -b
SMALL_SNO=snobol4 -b

################

AUX_OBJS= _OBJS
OBJS=	main.o $(SNOBOL4).o data.o data_init.o syn.o bal.o convert.o \
	date.o dump.o dynamic.o endex.o expops.o hash.o init.o intspc.o \
	io.o lexcmp.o load.o mstime.o ordvst.o pair.o pat.o pml.o \
	realst.o replace.o str.o stream.o term.o top.o tree.o version.o \
	$(PML_OBJS) $(SNOLIB_A)

AUX_SRCS= _SRCS
SRCS=	main.c $(SNOBOL4).c data.c data_init.c syn.c $(BAL_C) $(CONVERT_C) \
	$(DATE_C) $(DUMP_C) $(DYNAMIC_C) $(ENDEX_C) $(EXPOPS_C) $(HASH_C) \
	$(INIT_C) $(INTSPC_C) $(IO_C) $(LEXCMP_C) $(LOAD_C) \
	$(MSTIME_C) $(ORDVST_C) $(PAIR_C) $(PAT_C) $(PML_C) \
	$(REALST_C) $(REPLACE_C) $(STREAM_C) $(STR_C) $(TOP_C) $(TERM_C) \
	$(TREE_C) version.c $(AUX_SRCS) $(SNOLIB_SRCS)

# SIL source file
SIL=	v311.sil

################
# link, regression test & timing

TESTED:	xsnobol4 snobol4.c
	@echo Running regression tests...
	cd test; ./run.sh ../xsnobol4 > ../test.out
	./timing > timing.out
	date > TESTED

xsnobol4: $(OBJS)
	$(CC) $(CFLAGS) -o xsnobol4 $(OBJS) $(LDFLAGS)

################
# code

.PRECIOUS: snobol4.c isnobol4.c $(SNOBOL4).o data_init.o

# may need special options due to size!!
$(SNOBOL4).o: $(SNOBOL4).c 
	$(CC) $(SNOBOL4_C_CFLAGS) $(CFLAGS) -c $(SNOBOL4).c

# regular version
snobol4.c: procs genc.sno globals $(SIL) 
	rm -f snobol4.c2 proc.h2
	$(SNO) genc.sno < $(SIL) > snobol4.c2
	mv snobol4.c2 snobol4.c

# inline version (functions reordered)
isnobol4.c: procs genc.sno globals $(SIL) inline.sno
	rm -rf isnobol4.c2 proc.h2 prolog subr
	mkdir subr
	$(SNO) inline.sno < $(SIL) > prolog
	cd subr; cat ../prolog \
		`awk '{print $$2, $$1}' ../callgraph | tsort 2>/dev/null` \
			> ../isnobol4.c2
	mv isnobol4.c2 isnobol4.c
	rm -rf prolog subr

proc.h2: $(SNOBOL4).c

proc.h:	proc.h2
	cmp proc.h proc.h2 || cp proc.h2 proc.h

################
# syntax tables

# only change syn.h if it has changed from last run!

syn.c syn.h2: syntax.tbl gensyn.sno
	rm -f syn.c2 syn.h2
	$(SNO) gensyn.sno
	mv syn.c2 syn.c

syn.h:	syn.h2
	cmp syn.h syn.h2 || cp syn.h2 syn.h

################
# resident data

data.h2 data.c2 equ.h2 data_init.h2: $(SIL) gendata.sno
	rm -rf data.h2 data.c2 equ.h2 data_init.h2
	$(SNO) gendata.sno < $(SIL)

data.h:	data.h2
	cmp data.h data.h2 || cp data.h2 data.h

data.c:	data.c2
	cmp data.c data.c2 || cp data.c2 data.c

equ.h:	equ.h2
	cmp equ.h equ.h2 || cp equ.h2 equ.h

data_init.h: data_init.h2
	cmp data_init.h data_init.h2 || cp data_init.h2 data_init.h

# note: private CFLAGS
data_init.o: data_init.c data_init.h equ.h data.h proc.h
	$(CC) $(DATA_INIT_CFLAGS) -c data_init.c

#################
# dependency generation is slow and ugly.
# doesn't change much!!

#procs:	gendep.sno
#	$(SNO) gendep.sno < $(SIL) > procs2
#	mv procs2 procs

#################################################################
# lib files

bal.o:	$(BAL_C)
	$(CC) $(CFLAGS) -c $(BAL_C)

convert.o: $(CONVERT_C)
	$(CC) $(CFLAGS) -c $(CONVERT_C)

date.o:	$(DATE_C)
	$(CC) $(CFLAGS) -c $(DATE_C)

dump.o:	$(DUMP_C)
	$(CC) $(CFLAGS) -c $(DUMP_C)

dynamic.o: $(DYNAMIC_C)
	$(CC) $(CFLAGS) -c $(DYNAMIC_C)

endex.o: $(ENDEX_C)
	$(CC) $(CFLAGS) -c $(ENDEX_C)

expops.o: $(EXPOPS_C)
	$(CC) $(CFLAGS) -c $(EXPOPS_C)

hash.o:	$(HASH_C)
	$(CC) $(CFLAGS) -c $(HASH_C)

init.o:	$(INIT_C)
	$(CC) $(CFLAGS) -c $(INIT_C)

intspc.o: $(INTSPC_C)
	$(CC) $(CFLAGS) -c $(INTSPC_C)

io.o:	$(IO_C)
	$(CC) $(CFLAGS) $(SNOLIB_DEFINES) -c $(IO_C)

lexcmp.o: $(LEXCMP_C)
	$(CC) $(CFLAGS) -c $(LEXCMP_C)

load.o:	$(LOAD_C)
	$(CC) $(CFLAGS) $(SNOLIB_DEFINES) -c $(LOAD_C)

mstime.o: $(MSTIME_C)
	$(CC) $(CFLAGS) -c $(MSTIME_C)

ordvst.o: $(ORDVST_C)
	$(CC) $(CFLAGS) -c $(ORDVST_C)

pair.o:	$(PAIR_C)
	$(CC) $(CFLAGS) -c $(PAIR_C)

pat.o:	$(PAT_C)
	$(CC) $(CFLAGS) -c $(PAT_C)

pml.o:	$(PML_C)
	$(CC) $(CFLAGS) -c $(PML_C)

realst.o: $(REALST_C)
	$(CC) $(CFLAGS) -c $(REALST_C)

replace.o: $(REPLACE_C)
	$(CC) $(CFLAGS) -c $(REPLACE_C)

str.o:	$(STR_C)
	$(CC) $(CFLAGS) -c $(STR_C)

stream.o: $(STREAM_C)
	$(CC) $(CFLAGS) -c $(STREAM_C)

term.o:	$(TERM_C)
	$(CC) $(CFLAGS) -c $(TERM_C)

top.o:	$(TOP_C)
	$(CC) $(CFLAGS) -c $(TOP_C)

tree.o:	$(TREE_C)
	$(CC) $(CFLAGS) -c $(TREE_C)

#################
# aux files -- porting aids not used in all builds;

bzero.o: $(BZERO_C)
	$(CC) $(CFLAGS) -c $(BZERO_C)

bcopy.o: $(BCOPY_C)
	$(CC) $(CFLAGS) -c $(BCOPY_C)

getopt.o: $(GETOPT_C)
	$(CC) $(CFLAGS) -c $(GETOPT_C)

isnan.o: $(ISNAN_C)
	$(CC) $(CFLAGS) -c $(ISNAN_C)

################################################################
# snolib -- library of external functions

SNOLIB_OBJS= chop.o cos.o delete.o environ.o exit.o exp.o file.o \
	fork.o getstring.o host.o log.o rename.o retstring.o sin.o \
	sprintf.o sqrt.o tan.o $(AUX_OBJS)

$(SNOLIB_A): $(SNOLIB_OBJS)
	rm -f $(SNOLIB_A)
	ar rv $(SNOLIB_A) $(SNOLIB_OBJS)
	$(RANLIB) $(SNOLIB_A)

################
# snolib files

SNOLIB_SRCS= $(CHOP_C) $(COS_C) $(DELETE_C) $(ENVIRON_C) $(EXIT_C) \
	$(EXP_C) $(FILE_C) $(FORK_C) $(GETSTRING_C) $(HOST_C) $(LOG_C) \
	$(RENAME_C) $(RETSTRING_C) $(SIN_C) $(SPRINTF_C) $(SQRT_C) \
	$(TAN_C)

chop.o: $(CHOP_C)
	$(CC) $(CFLAGS) -c $(CHOP_C)

cos.o: $(COS_C)
	$(CC) $(CFLAGS) -c $(COS_C)

delete.o: $(DELETE_C)
	$(CC) $(CFLAGS) -c $(DELETE_C)

environ.o: $(ENVIRON_C)
	$(CC) $(CFLAGS) -c $(ENVIRON_C)

exit.o: $(EXIT_C)
	$(CC) $(CFLAGS) -c $(EXIT_C)

exp.o: $(EXP_C)
	$(CC) $(CFLAGS) -c $(EXP_C)

file.o: $(FILE_C)
	$(CC) $(CFLAGS) -c $(FILE_C)

fork.o: $(FORK_C)
	$(CC) $(CFLAGS) -c $(FORK_C)

getstring.o: $(GETSTRING_C)
	$(CC) $(CFLAGS) -c $(GETSTRING_C)

host.o: $(HOST_C)
	$(CC) $(CFLAGS) -c $(HOST_C)

log.o: $(LOG_C)
	$(CC) $(CFLAGS) -c $(LOG_C)

rename.o: $(RENAME_C)
	$(CC) $(CFLAGS) -c $(RENAME_C)

retstring.o: $(RETSTRING_C)
	$(CC) $(CFLAGS) -c $(RETSTRING_C)

sin.o: $(SIN_C)
	$(CC) $(CFLAGS) -c $(SIN_C)

sprintf.o: $(SPRINTF_C)
	$(CC) $(CFLAGS) -c $(SPRINTF_C)

sqrt.o: $(SQRT_C)
	$(CC) $(CFLAGS) -c $(SQRT_C)

tan.o: $(TAN_C)
	$(CC) $(CFLAGS) -c $(TAN_C)

##################################################################
# housekeeping

# generated files to include in kit
GENERATED=syn.c syn.h data.c data.h data_init.h proc.h equ.h \
	snobol4.c isnobol4.c 

# files with secondary copies
G2=data.c data.h data_init.h proc.h equ.h syn.h

# disposables
DISP=*.o *.a callgraph prolog subr

# remove objects; leave generated sources, final binary, Makefile2
clean:
	rm -rf $(DISP) *~ */*~ */*/*~ *.tmp

# remove objects, generated sources; leave final binary, Makefile2
# DON'T DO THIS UNLESS YOU HAVE AN EXECUTABLE!!
realclean: clean
	rm -f $(GENERATED)

# file to hard-link into dist dir
# generated files copied to ensure newer than source files!
[TAR=	README doc History TODO TODO.soon \
	Makefile Makefile2.m4 autoconf config.guess \
	$(SIL) syntax.tbl procs globals \
	genc.sno gensyn.sno gendata.sno inline.sno \
	main.c charset.c data_init.c version.c \
	parms.h mlink.h mdata.h pml.h \
	lib include config test bugs \
	snolib/Makefile snolib/*.[ch] snolib/*.sno \
	timing timing.sno \
	cc-M]

# "print version" -- for dir/tar names
pv:	version.c
	$(CC) -I[./include] -DMAIN -o pv version.c

# compression program, suffix
COMP=gzip
Z=gz

VERS=`./pv`
DIR=snobol-$(VERS)
KIT=snobol-$(VERS).tar.$(Z)

# XXX perform general cleanup (remove ~ and # files) first?
# XXX add predicates to suppress ~ # and .o files?
tar vers: TESTED pv
	cd doc; make
	cd test; ./clean.sh
	rm -rf [snobol-[0-9]*]
	rm -f *~ */*~ */*/*~ *.tmp
	mkdir $(DIR)
	find $(TAR) -name RCS -prune -o -print | cpio -pldm $(DIR)
	for f in $(G2); do cp $$f $(DIR)/$${f}2; done
	cp $(GENERATED) $(DIR)
	tar cf - $(DIR) | $(COMP) > $(KIT)
	rm -rf $(DIR)
	./pv > vers

# make a mailable kit by taring, compressing, uuencoding and splitting!
uu:	vers
	rm -rf uu; mkdir uu
	cd uu; uuencode ../$(TAR) $(TAR) | split -800 -; makekit x??; rm x??

#################

MAKEFILE2=Makefile2
depend:
	sed '/^# DO NOT DELETE THIS LINE/q' $(MAKEFILE2) > $(MAKEFILE2).tmp
	$(CCM) $(MYCPPFLAGS) $(SRCS) >> $(MAKEFILE2).tmp
	mv $(MAKEFILE2).tmp $(MAKEFILE2)

