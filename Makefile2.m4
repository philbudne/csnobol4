# $Id$
changequote([,])dnl
define([CMT],[dnl])dnl
CMT([################################################################])
CMT([establish m4 macros to collect various options])
CMT()
CMT([extra C compiler cpp flags for all .c files & make depend])
define([ADD_CPPFLAGS],[define([_CPPFLAGS],_CPPFLAGS $1)dnl])dnl
define([_CPPFLAGS],)dnl
CMT()
CMT([extra C compiler optimize/debug flags for all .c files])
define([ADD_OPT],[define([_OPT],_OPT $1)dnl])dnl
define([_OPT],)dnl
CMT()
CMT([extra source files to make depend])
define([ADD_SRCS],[define([_SRCS],_SRCS $1)dnl])dnl
define([_SRCS],)dnl
CMT()
CMT([extra object files to snolib])
define([ADD_OBJS],[define([_OBJS],_OBJS $1)dnl])dnl
define([_OBJS],)dnl
CMT()
CMT([extra C compiler flags for final link])
define([ADD_LDFLAGS],[define([_LDFLAGS],_LDFLAGS $1)dnl])dnl
define([_LDFLAGS],)dnl

################################################################
# defaults (may be overridden in config.m4)

OPT=-O

CCM=./cc-M

# for pow(3)
MATHLIB=-lm

SH=sh
SHELL=/bin/sh

# library random accessifier
RANLIB=ranlib

# filename for external function library
SNOLIB_A=snolib.a

# default file to load
SNOLIB_FILE=snolib.a

# directory name for default SNOLIB (used by -INCLUDE and LOAD())
SNOLIB_DIR=${PREFIX}/lib/snobol4

# default name for installed binary
BINDEST=${PREFIX}/bin/snobol4

# default name for installed man page
MANDEST=${PREFIX}/man/man1/snobol4.1

# directory prefix
PREFIX=/usr/local

# either snobol4 or isnobol4;
# isnobol4 has had functions reordered for better inlining.
# if compiler does not perform inlining, snobol4 can be used
# with no penalty (and slightly simpler build process).

SNOBOL4=isnobol4
#SNOBOL4=snobol4

########
# default lib source files

BAL_C=$(SRCDIR)lib/bal.c
DATE_C=$(SRCDIR)lib/date.c
DUMP_C=$(SRCDIR)lib/dump.c
DYNAMIC_C=$(SRCDIR)lib/bsd/dynamic.c
ENDEX_C=$(SRCDIR)lib/endex.c
EXISTS_C=$(SRCDIR)lib/generic/exists.c
EXPOPS_C=$(SRCDIR)lib/generic/expops.c
HASH_C=$(SRCDIR)lib/hash.c
INET_C=$(SRCDIR)lib/bsd/inet.c
INIT_C=$(SRCDIR)lib/init.c
INTSPC_C=$(SRCDIR)lib/generic/intspc.c
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
SPCINT_C=$(SRCDIR)lib/ansi/spcint.c
SPREAL_C=$(SRCDIR)lib/ansi/spreal.c
STREAM_C=$(SRCDIR)lib/stream.c
SUSPEND_C=$(SRCDIR)lib/posix/suspend.c
STR_C=$(SRCDIR)lib/str.c
TERM_C=$(SRCDIR)lib/posix/term.c
TOP_C=$(SRCDIR)lib/top.c
TREE_C=$(SRCDIR)lib/tree.c
TTY_C=$(SRCDIR)lib/posix/tty.c

# aux sources
BZERO_C=$(SRCDIR)lib/auxil/bzero.c
BCOPY_C=$(SRCDIR)lib/auxil/bcopy.c
GETOPT_C=$(SRCDIR)lib/auxil/getopt.c

# dummy sources
EXECL_C=$(SRCDIR)lib/dummy/execl.c
FINITE_C=$(SRCDIR)lib/dummy/finite.c
GETENV_C=$(SRCDIR)lib/dummy/getenv.c
ISNAN_C=$(SRCDIR)lib/dummy/isnan.c
SYSTEM_C=$(SRCDIR)lib/dummy/system.c

# snolib sources
CHOP_C=$(SRCDIR)lib/snolib/chop.c
COS_C=$(SRCDIR)lib/snolib/cos.c
DELETE_C=$(SRCDIR)lib/snolib/delete.c
ENVIRON_C=$(SRCDIR)lib/snolib/environ.c
EXECUTE_C=$(SRCDIR)lib/generic/execute.c
EXIT_C=$(SRCDIR)lib/snolib/exit.c
EXP_C=$(SRCDIR)lib/snolib/exp.c
FILE_C=$(SRCDIR)lib/snolib/file.c
FINDUNIT_C=$(SRCDIR)lib/snolib/findunit.c
FORK_C=$(SRCDIR)lib/snolib/fork.c
GETSTRING_C=$(SRCDIR)lib/snolib/getstring.c
HOST_C=$(SRCDIR)lib/snolib/host.c
LOG_C=$(SRCDIR)lib/snolib/log.c
ORD_C=$(SRCDIR)lib/snolib/ord.c
RENAME_C=$(SRCDIR)lib/snolib/rename.c
RETSTRING_C=$(SRCDIR)lib/snolib/retstring.c
SERV_C=$(SRCDIR)lib/snolib/serv.c
SIN_C=$(SRCDIR)lib/snolib/sin.c
SPRINTF_C=$(SRCDIR)lib/snolib/sprintf.c
SQRT_C=$(SRCDIR)lib/snolib/sqrt.c
SSET_C=$(SRCDIR)lib/snolib/sset.c
SYS_C=$(SRCDIR)lib/posix/sys.c
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

SNOLIB_DEFINES=-DSNOLIB_DIR='"'$(SNOLIB_DIR)'"' \
	-DSNOLIB_FILE='"'$(SNOLIB_FILE)'"'

################
# compiler flags

CFLAGS=$(COPT) $(MYCPPFLAGS)

################

# XXX replace SNOLIB_A with SNOLIB_FILE??
#	need to add rules to make shared libraries (to config/xxx.m4 files)

OBJS=	main.o $(SNOBOL4).o data.o data_init.o syn.o bal.o date.o \
	dump.o dynamic.o endex.o expops.o hash.o inet.o init.o \
	intspc.o io.o lexcmp.o load.o mstime.o ordvst.o pair.o pat.o \
	pml.o realst.o replace.o spcint.o spreal.o str.o stream.o \
	suspend.o term.o top.o tree.o tty.o version.o \
	$(PML_OBJS) $(SNOLIB_A)

AUX_SRCS= _SRCS
SRCS=	main.c $(SNOBOL4).c data.c data_init.c syn.c $(BAL_C) \
	$(DATE_C) $(DUMP_C) $(DYNAMIC_C) $(ENDEX_C) $(EXPOPS_C) \
	$(HASH_C) $(INET_C) $(INIT_C) $(INTSPC_C) $(IO_C) $(LEXCMP_C) \
	$(LOAD_C) $(MSTIME_C) $(ORDVST_C) $(PAIR_C) $(PAT_C) $(PML_C) \
	$(REALST_C) $(REPLACE_C) $(SPCINT_C) $(SPREAL_C) $(STREAM_C) \
	$(STR_C) $(SUSPEND_C) $(TOP_C) $(TERM_C) $(TREE_C) $(TTY_C) \
	version.c $(AUX_SRCS) $(SNOLIB_SRCS)

################
# link, regression test & timing

.PRECIOUS: $(SNOBOL4).o data_init.o snobol4

changequote(@,@)dnl

xsnobol4: $(OBJS)
	rm -f build.c
	echo '/* MACHINE GENERATED.  EDITING IS FUTILE */'	> build.c
	echo 'const char build_files[] = "'$(SRCS)'";'		>> build.c
	echo 'const char build_date[] = "'`date`'";'		>> build.c
	echo 'const char build_dir[] = "'`pwd`'";'		>> build.c
	$(CC) $(CFLAGS) -c build.c
	rm -f xsnobol4$(EXT)
	$(CC) $(CFLAGS) -o xsnobol4 $(OBJS) build.o $(LDFLAGS)

changequote([,])dnl

################
# run regression tests.

snobol4: xsnobol4 snobol4.c
	@echo Running regression tests...
	cd test; ./run.sh ../xsnobol4
	@echo Passed regression tests.
	./timing > timing.out
	-rm -f snobol4$(EXT)
	ln xsnobol4$(EXT) snobol4$(EXT)
	@touch snobol4$(EXT)
	@echo '***************************************************************'
	@echo Please consider mailing timing.out to \
		snobol4-timing@ultimate.com 1>&2
	@echo along with information on your \
		system model and CPU clock rate 1>&2
	@echo see doc/ports.doc for the current list 1>&2
	@echo '***************************************************************'

################

# may need additional options due to size!!
$(SNOBOL4).o: $(SRCDIR)$(SNOBOL4).c 
	$(CC) $(SNOBOL4_C_CFLAGS) $(CFLAGS) -c $(SRCDIR)$(SNOBOL4).c

version.o: $(SRCDIR)version.c
	$(CC) $(CFLAGS) -c $(SRCDIR)version.c

main.o: $(SRCDIR)main.c
	$(CC) $(CFLAGS) -c $(SRCDIR)main.c

data.o: $(SRCDIR)data.c
	$(CC) $(CFLAGS) -c $(SRCDIR)data.c

# NOTE: private options; includes data_init.h, which is huge and tends
# to gum up the optimizer.  For development, I keep a local-config
# file with the line "DATA_INIT_CFLAGS=$(MYCPPFLAGS)" so that no
# attempt to optimize compilation of data_init.c occurs.  This may
# result in a slightly larger executable, or slower startup, but since
# I may compile data_init.c many times in the course of a debug
# session, it's worth it.
data_init.o: $(SRCDIR)data_init.c 
	$(CC) $(DATA_INIT_CFLAGS) -c $(SRCDIR)data_init.c

syn.o: $(SRCDIR)syn.c
	$(CC) $(CFLAGS) -c $(SRCDIR)syn.c

#################################################################
# lib files

bal.o:	$(BAL_C)
	$(CC) $(CFLAGS) -c $(BAL_C)

date.o:	$(DATE_C)
	$(CC) $(CFLAGS) -c $(DATE_C)

dump.o:	$(DUMP_C)
	$(CC) $(CFLAGS) -c $(DUMP_C)

dynamic.o: $(DYNAMIC_C)
	$(CC) $(CFLAGS) -c $(DYNAMIC_C)

endex.o: $(ENDEX_C)
	$(CC) $(CFLAGS) -c $(ENDEX_C)

exists.o: $(EXISTS_C)
	$(CC) $(CFLAGS) -c $(EXISTS_C)

expops.o: $(EXPOPS_C)
	$(CC) $(CFLAGS) -c $(EXPOPS_C)

hash.o:	$(HASH_C)
	$(CC) $(CFLAGS) -c $(HASH_C)

inet.o:	$(INET_C)
	$(CC) $(CFLAGS) -c $(INET_C)

init.o:	$(INIT_C)
	$(CC) $(CFLAGS) -c $(INIT_C)

intspc.o: $(INTSPC_C)
	$(CC) $(CFLAGS) -c $(INTSPC_C)

io.o:	$(IO_C) $(MAKEFILE2)
	$(CC) $(CFLAGS) $(SNOLIB_DEFINES) -c $(IO_C)

lexcmp.o: $(LEXCMP_C)
	$(CC) $(CFLAGS) -c $(LEXCMP_C)

load.o:	$(LOAD_C) $(MAKEFILE2)
	$(CC) $(CFLAGS) $(SNOLIB_DEFINES) $(LD_PATH) -c $(LOAD_C)

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

spcint.o: $(SPCINT_C)
	$(CC) $(CFLAGS) -c $(SPCINT_C)

spreal.o: $(SPREAL_C)
	$(CC) $(CFLAGS) -c $(SPREAL_C)

str.o:	$(STR_C)
	$(CC) $(CFLAGS) -c $(STR_C)

stream.o: $(STREAM_C)
	$(CC) $(CFLAGS) -c $(STREAM_C)

suspend.o: $(SUSPEND_C)
	$(CC) $(CFLAGS) -c $(SUSPEND_C)

term.o:	$(TERM_C)
	$(CC) $(CFLAGS) -c $(TERM_C)

top.o:	$(TOP_C)
	$(CC) $(CFLAGS) -c $(TOP_C)

tree.o:	$(TREE_C)
	$(CC) $(CFLAGS) -c $(TREE_C)

tty.o:	$(TTY_C)
	$(CC) $(CFLAGS) -c $(TTY_C)

#################
# aux files -- porting aids not used in all builds;

bzero.o: $(BZERO_C)
	$(CC) $(CFLAGS) -c $(BZERO_C)

bcopy.o: $(BCOPY_C)
	$(CC) $(CFLAGS) -c $(BCOPY_C)

getopt.o: $(GETOPT_C)
	$(CC) $(CFLAGS) -c $(GETOPT_C)

finite.o: $(FINITE_C)
	$(CC) $(CFLAGS) -c $(FINITE_C)

################
# dummy files

isnan.o: $(ISNAN_C)
	$(CC) $(CFLAGS) -c $(ISNAN_C)

# for snolib/host.c
getenv.o: $(GETENV_C)
	$(CC) $(CFLAGS) -c $(GETENV_C)

# for snolib/host.c
system.o: $(SYSTEM_C)
	$(CC) $(CFLAGS) -c $(SYSTEM_C)

################################################################
# snolib -- library of external functions

AUX_OBJS= _OBJS

SNOLIB_OBJS= chop.o cos.o delete.o environ.o execute.o exists.o exit.o \
	exp.o file.o findunit.o fork.o getstring.o host.o ord.o log.o \
	rename.o retstring.o serv.o sin.o sprintf.o sqrt.o sset.o sys.o \
	tan.o $(AUX_OBJS)

$(SNOLIB_A): $(SNOLIB_OBJS)
	rm -f $(SNOLIB_A)
	ar $(ARXFLAGS) rv $(SNOLIB_A) $(SNOLIB_OBJS)
	$(RANLIB) $(SNOLIB_A)

################
# snolib files

SNOLIB_SRCS= $(CHOP_C) $(COS_C) $(DELETE_C) $(ENVIRON_C) $(EXISTS_C) \
	$(EXIT_C) $(EXECUTE_C) $(EXP_C) $(FILE_C) $(FORK_C) $(GETSTRING_C) \
	$(HOST_C) $(LOG_C) $(RENAME_C) $(RETSTRING_C) $(SERV_C) $(SIN_C) \
	$(SPRINTF_C) $(SQRT_C) $(SYS_C) $(TAN_C)

chop.o: $(CHOP_C)
	$(CC) $(CFLAGS) -c $(CHOP_C)

cos.o: $(COS_C)
	$(CC) $(CFLAGS) -c $(COS_C)

delete.o: $(DELETE_C)
	$(CC) $(CFLAGS) -c $(DELETE_C)

environ.o: $(ENVIRON_C)
	$(CC) $(CFLAGS) -c $(ENVIRON_C)

execute.o: $(EXECUTE_C)
	$(CC) $(CFLAGS) -c $(EXECUTE_C)

exit.o: $(EXIT_C)
	$(CC) $(CFLAGS) -c $(EXIT_C)

exp.o: $(EXP_C)
	$(CC) $(CFLAGS) -c $(EXP_C)

file.o: $(FILE_C)
	$(CC) $(CFLAGS) -c $(FILE_C)

findunit.o: $(FINDUNIT_C)
	$(CC) $(CFLAGS) -c $(FINDUNIT_C)

fork.o: $(FORK_C)
	$(CC) $(CFLAGS) -c $(FORK_C)

getstring.o: $(GETSTRING_C)
	$(CC) $(CFLAGS) -c $(GETSTRING_C)

host.o: $(HOST_C)
	$(CC) $(CFLAGS) $(SNOLIB_DEFINES) -c $(HOST_C)

log.o: $(LOG_C)
	$(CC) $(CFLAGS) -c $(LOG_C)

ord.o: $(ORD_C)
	$(CC) $(CFLAGS) -c $(ORD_C)

rename.o: $(RENAME_C)
	$(CC) $(CFLAGS) -c $(RENAME_C)

retstring.o: $(RETSTRING_C)
	$(CC) $(CFLAGS) -c $(RETSTRING_C)

serv.o: $(SERV_C)
	$(CC) $(CFLAGS) -c $(SERV_C)

sin.o: $(SIN_C)
	$(CC) $(CFLAGS) -c $(SIN_C)

sprintf.o: $(SPRINTF_C)
	$(CC) $(CFLAGS) -c $(SPRINTF_C)

sqrt.o: $(SQRT_C)
	$(CC) $(CFLAGS) -c $(SQRT_C)

sset.o: $(SSET_C)
	$(CC) $(CFLAGS) -c $(SSET_C)

sys.o: $(SYS_C)
	$(CC) $(CFLAGS) -c $(SYS_C)

tan.o: $(TAN_C)
	$(CC) $(CFLAGS) -c $(TAN_C)

#################
# installation

# install .h files for dynamicly loaded functions
INSTALL_H=[include]/h.h [include]/snotypes.h [include]/macros.h \
	[include]/load.h [include]/dt.h config.h

# generated SNOLIB files (at top level)
GENSNOLIB=host.sno

install: snobol4 doc/snobol4.1
	-rm -f $(BINDEST)
	cp snobol4 $(BINDEST); strip $(BINDEST); chmod 755 $(BINDEST)
	-rm -f $(BINDEST)-$(VERS)
	ln $(BINDEST) $(BINDEST)-$(VERS)
	rm -f $(MANDEST)
	cp doc/snobol4.1 $(MANDEST)
	test -d $(SNOLIB_DIR) || mkdir $(SNOLIB_DIR)
	for F in snolib/*.sno $(INSTALL_H) doc/load.doc $(GENSNOLIB); do \
		rm -f $(SNOLIB_DIR)/`basename $$F`; cp $$F $(SNOLIB_DIR); \
	done
	@echo '************************************************'
	@echo '*** Have you mailed a copy of timing.out to' \
		'snobol4-timing@ultimate.com ?' 1>&2
	
################
MAKEFILE2=Makefile2
DEPENDFLAGS=$(MYCPPFLAGS)

depend:
	sed '/^# DO NOT DELETE THIS LINE/q' $(MAKEFILE2) > $(MAKEFILE2).tmp
	$(CCM) $(DEPENDFLAGS) $(SRCS) >> $(MAKEFILE2).tmp
	mv -f $(MAKEFILE2).tmp $(MAKEFILE2)
