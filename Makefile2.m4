# $Id$

################################################################
# defaults (may be overridden in config.h)

# GCC is preferred C compiler; turn on inlining!
CC=gcc
OPT=-O -finline-functions

# May substitute ./cc-M here if C compiler doesn't support -M flag
CCM=$(CC) -M

# for pow(3)
MATHLIB=-lm

########
# default lib source files

BAL_C=lib/bal.c
CONVERT_C=lib/ansi/convert.c
DATE_C=lib/date.c
DUMP_C=lib/dump.c
DYNAMIC_C=lib/bsd/dynamic.c
ENDEX_C=lib/endex.c
EXP_C=lib/generic/exp.c
HASH_C=lib/hash.c
INIT_C=lib/init.c
INTSPC_C=lib/intspc.c
IO_C=lib/io.c
LEXCMP_C=lib/lexcmp.c
LOAD_C=lib/bsd/load.c
MSTIME_C=lib/bsd/mstime.c
ORDVST_C=lib/ordvst.c
PAIR_C=lib/pair.c
PAT_C=lib/pat.c
PML_C=lib/pml.c
REALST_C=lib/realst.c
REPLACE_C=lib/replace.c
STREAM_C=lib/stream.c
STR_C=lib/str.c
TOP_C=lib/top.c
TREE_C=lib/tree.c

# aux sources
BZERO_C=lib/aux/bzero.c
BCOPY_C=lib/aux/bcopy.c
GETOPT_C=lib/aux/getopt.c
VFPRINTF_C=lib/aux/vfprintf.c

# end of defaults
################################################################

# include local config (copied or linked from config/ directory)
include(config.m4)

# end of local config
################################################################

# NOTE: NOT named CPPFLAGS; some versions of make include CPPFLAGS in cc cmd
MYCPPFLAGS=`-I./include -I. $(CONFIG_CPPFLAGS)'

CFLAGS=$(OPT) $(MYCPPFLAGS) $(CONFIG_CFLAGS)

LIBS=$(MATHLIB)

LDFLAGS=$(LIBS) $(CONFIG_LDFLAGS)

# bootstrapped using Catspaw SPARC SPITBOL
#SNO=spitbol -i512k -b
#SMALL_SNO=spitbol -b
SNO=snobol4 -d50000
SMALL_SNO=snobol4

################
# either snobol4 or isnobol4
SNOBOL4=snobol4

.PRECIOUS: $(SNOBOL4).c $(SNOBOL4).o data_init.o

OBJS=	main.o $(SNOBOL4).o data.o data_init.o syn.o bal.o convert.o \
	date.o dump.o dynamic.o endex.o exp.o hash.o init.o intspc.o \
	io.o lexcmp.o load.o mstime.o ordvst.o pair.o pat.o pml.o \
	realst.o replace.o str.o stream.o top.o tree.o version.o \
	$(CONFIG_OBJ) $(AUX_OBJ) $(PML_OBJ)

SRCS=	main.c $(SNOBOL4).c data.c data_init.c $(BAL_C) $(CONVERT_C) \
	$(DATE_C) $(DUMP_C) $(DYNAMIC_C) $(ENDEX_C) $(EXP_C) $(HASH_C) \
	$(INIT_C) $(INTSPC_C) $(IO_C) $(LEXCMP_C) $(LOAD_C) \
	$(MSTIME_C) $(ORDVST_C) $(PAIR_C) $(PAT_C) $(PML_C) \
	$(REALST_C) $(REPLACE_C) $(STREAM_C) $(STR_C) $(TOP_C) \
	$(TREE_C) version.c $(CONFIG_SRC) $(AUX_SRC)

TESTED:	xsnobol4
	@echo Running regression tests...
	cd test; ./run.sh ../xsnobol4 > test.out
	./timing > timing.out
	date > TESTED

xsnobol4: $(OBJS)
	$(CC) $(CFLAGS) -o xsnobol4 $(OBJS) $(LDFLAGS)

################

# may need special options due to size!!
$(SNOBOL4).o: $(SNOBOL4).c 
	$(CC) $(CFLAGS) $(SNOBOL4_C_CFLAGS) -c $(SNOBOL4).c

proc.h:	$(SNOBOL4).c

# regular version
snobol4.c: procs genc.sno global.procs v311.sil 
	rm -f snobol4.c.TMP
	$(SNO) genc.sno > snobol4.c.TMP
	mv snobol4.c.TMP snobol4.c

changequote(%,@)
# inline version (functions reordered)
isnobol4.c: procs genc.sno global.procs v311.sil inline.sno reverse
	rm -rf isnobol4.c.TMP prolog subr
	mkdir subr
	$(SNO) inline.sno > prolog
	cd subr; cat ../prolog `tsort ../callgraph 2>/dev/null | reverse` \
				> ../isnobol4.c.TMP
	mv isnobol4.c.TMP isnobol4.c
	rm -rf prolog subr
changequote()

reverse: reverse.c
	$(CC) $(CFLAGS) -o reverse reverse.c

################

# only change syn.h if it has changed from last run!

syn.c syn.h2: syntax.tbl gensyn.sno
	$(SNO) gensyn.sno

syn.h:	syn.h2
	cmp syn.h syn.h2 || cp syn.h2 syn.h

################

data.h data.c equ.h data_init.h: v311.sil gendata.sno
	$(SNO) gendata.sno

data_init.o:	data_init.c data_init.h equ.h data.h proc.h

#################
# dependency generation is slow and ugly.
# doesn't change much!!

#procs:	gendep.sno
#	$(SNO) gendep.sno > procs.TMP
#	mv procs.TMP procs

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

exp.o:	$(EXP_C)
	$(CC) $(CFLAGS) -c $(EXP_C)

hash.o:	$(HASH_C)
	$(CC) $(CFLAGS) -c $(HASH_C)

init.o:	$(INIT_C)
	$(CC) $(CFLAGS) -c $(INIT_C)

intspc.o: $(INTSPC_C)
	$(CC) $(CFLAGS) -c $(INTSPC_C)

io.o:	$(IO_C)
	$(CC) $(CFLAGS) -c $(IO_C)

lexcmp.o: $(LEXCMP_C)
	$(CC) $(CFLAGS) -c $(LEXCMP_C)

load.o:	$(LOAD_C)
	$(CC) $(CFLAGS) -c $(LOAD_C)

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

top.o:	$(TOP_C)
	$(CC) $(CFLAGS) -c $(TOP_C)

tree.o:	$(TREE_C)
	$(CC) $(CFLAGS) -c $(TREE_C)

#################
# porting aids not used in all builds;

bzero.o: $(BZERO_C)
	$(CC) $(CFLAGS) -c $(BZERO_C)

bcopy.o: $(BCOPY_C)
	$(CC) $(CFLAGS) -c $(BCOPY_C)

getopt.o: $(GETOPT_C)
	$(CC) $(CFLAGS) -c $(GETOPT_C)

vfprintf.o: $(VFPRINTF_C)
	$(CC) $(CFLAGS) -c $(VFPRINTF_C)

##################################################################
# housekeeping

# generated files to include in kit
GENERATED=syn.c syn.h syn.h2 data.c data.h proc.h equ.h \
	snobol4.c isnobol4.c data_init.h 

# disposables
G2=*.o callgraph reverse prolog subr

# remove objects, leave generated sources, final binary, Makefile2
clean:
	rm -rf $(G2) *~

# remove objects, generated sources, leave final binary, Makefile2
realclean: clean
	rm -f $(GENERATED)

`TAR=	README doc History TODO TODO.soon \
	Makefile Makefile2.m4 \
	v311.sil syntax.tbl procs global.procs \
	genc.sno gensyn.sno gendata.sno inline.sno \
	main.c charset.c data_init.c version.c \
	parms.h mlink.h mdata.h pml.h \
	lib include config test bugs snolib \
	timing timing.sno \
	$(GENERATED) \
	cc-M'

# XXX perform general cleanup (remove ~ and # files) first?
KIT=snobol.tar.Z
tar:	$(KIT)

$(KIT):	$(TAR)
	cd doc; make
	cd test; ./clean.sh
	rm -rf snobol
	mkdir snobol
	find $(TAR) -name RCS -prune -o -print | cpio -pldm snobol
	tar cf - snobol | compress > $(KIT)
	rm -rf snobol

# make a mailable kit by taring, compressing, uuencoding and splitting!
uu:	$(KIT)
	rm -rf uu; mkdir uu
	cd uu; uuencode ../$(KIT) $(KIT) | split -800 -; makekit x??; rm x??

#################

MAKEFILE2=Makefile2
depend:
	sed '/^# DO NOT DELETE THIS LINE/q' $(MAKEFILE2) > $(MAKEFILE2).tmp
	$(CCM) $(MYCPPFLAGS) $(SRCS) >> $(MAKEFILE2).tmp
	mv $(MAKEFILE2).tmp $(MAKEFILE2)

