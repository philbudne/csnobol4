# $Id$
changequote([,])dnl
define([CMT],[dnl])dnl
CMT([################################################################])
CMT([establish m4 macros to collect various options])
CMT()
CMT([extra C compiler cpp flags for all .c files & make depend])
define([ADD_CPPFLAGS],[divert(1) $1[]divert(0)dnl])dnl
define([_CPPFLAGS],[undivert(1)])dnl
CMT()
CMT([extra C compiler optimize/debug flags for all .c files])
define([ADD_OPT],[divert(2) $1[]divert(0)dnl])dnl
define([_OPT],[undivert(2)])dnl
CMT()
CMT([extra source files to make depend])
define([ADD_SRCS],[divert(3) $1[]divert(0)dnl])dnl
define([_SRCS],[undivert(3)])dnl
CMT()
CMT([extra object files to snolib])
define([ADD_OBJS],[divert(4) $1[]divert(0)dnl])dnl
define([_OBJS],[undivert(4)])dnl
CMT()
CMT([extra C compiler flags for final link])
define([ADD_LDFLAGS],[divert(5) $1[]divert(0)dnl])dnl
define([_LDFLAGS],[undivert(5)])dnl
CMT()
CMT([extra C compiler flags for all .c files])
define([ADD_CFLAGS],[divert(6) $1[]divert(0)dnl])dnl
define([_CFLAGS],[undivert(6)])dnl

################################################################
# defaults (may be overridden in config.m4)

OPT=-O

CCM=./cc-M

# for pow(3)
MATHLIB=-lm

SH=sh
SHELL=/bin/sh

# either snobol4 or isnobol4;
# isnobol4 has had functions reordered for better inlining.
# if compiler does not perform inlining, snobol4 can be used
# with no penalty (and slightly simpler build process).

SNOBOL4=isnobol4
#SNOBOL4=snobol4

# default flags for install of binaries:
INSTALL_BIN_FLAGS=-s

all:	build_all timing.out

########
# default lib source files

BAL_C=$(SRCDIR)lib/bal.c
BREAK_C=$(SRCDIR)lib/break.c
DATE_C=$(SRCDIR)lib/date.c
DUMP_C=$(SRCDIR)lib/dump.c
DYNAMIC_C=$(SRCDIR)lib/bsd/dynamic.c
ENDEX_C=$(SRCDIR)lib/endex.c
EXISTS_C=$(SRCDIR)lib/generic/exists.c
EXPOPS_C=$(SRCDIR)lib/generic/expops.c
FISATTY_C=$(SRCDIR)lib/generic/fisatty.c
HASH_C=$(SRCDIR)lib/hash.c
INET_C=$(SRCDIR)lib/bsd/inet.c
INET6_C=$(SRCDIR)lib/bsd/inet6.c
INIT_C=$(SRCDIR)lib/init.c
INTSPC_C=$(SRCDIR)lib/generic/intspc.c
IO_C=$(SRCDIR)lib/io.c
LEXCMP_C=$(SRCDIR)lib/lexcmp.c
LOAD_C=$(SRCDIR)lib/bsd/load.c
LOADX_C=$(SRCDIR)lib/loadx.c
MSTIME_C=$(SRCDIR)lib/bsd/mstime.c
ORDVST_C=$(SRCDIR)lib/ordvst.c
PAIR_C=$(SRCDIR)lib/pair.c
PAT_C=$(SRCDIR)lib/pat.c
PML_C=$(SRCDIR)lib/pml.c
POPEN_C=$(SRCDIR)lib/bsd/popen.c
PTYIO_OBJ_C=$(SRCDIR)lib/dummy/ptyio_obj.c
REALST_C=$(SRCDIR)lib/realst.c
REPLACE_C=$(SRCDIR)lib/replace.c
SPCINT_C=$(SRCDIR)lib/ansi/spcint.c
SPREAL_C=$(SRCDIR)lib/ansi/spreal.c
STDIO_OBJ_C=$(SRCDIR)lib/stdio_obj.c
STREAM_C=$(SRCDIR)lib/stream.c
SUSPEND_C=$(SRCDIR)lib/posix/suspend.c
STR_C=$(SRCDIR)lib/str.c
TERM_C=$(SRCDIR)lib/posix/term.c
TOP_C=$(SRCDIR)lib/top.c
TREE_C=$(SRCDIR)lib/tree.c
TTY_C=$(SRCDIR)lib/posix/tty.c
# please keep in alphabetical order!!

# aux sources
BZERO_C=$(SRCDIR)lib/auxil/bzero.c
BCOPY_C=$(SRCDIR)lib/auxil/bcopy.c
GETOPT_C=$(SRCDIR)lib/auxil/getopt.c
BINDRESVPORT_C=$(SRCDIR)lib/auxil/bindresvport.c
GETLINE_C=$(SRCDIR)lib/auxil/getline.c
BUFIO_OBJ_C=$(SRCDIR)lib/auxil/bufio_obj.c
MEMIO_OBJ_C=$(SRCDIR)lib/auxil/memio_obj.c

# dummy sources
EXECL_C=$(SRCDIR)lib/dummy/execl.c
FINITE_C=$(SRCDIR)lib/dummy/finite.c
GETENV_C=$(SRCDIR)lib/dummy/getenv.c
ISNAN_C=$(SRCDIR)lib/dummy/isnan.c
SYSTEM_C=$(SRCDIR)lib/dummy/system.c

# snolib sources
ATAN_C=$(SRCDIR)lib/snolib/atan.c
CHOP_C=$(SRCDIR)lib/snolib/chop.c
COS_C=$(SRCDIR)lib/snolib/cos.c
DELETE_C=$(SRCDIR)lib/snolib/delete.c
ENVIRON_C=$(SRCDIR)lib/snolib/environ.c
EXECUTE_C=$(SRCDIR)lib/generic/execute.c
EXIT_C=$(SRCDIR)lib/snolib/exit.c
EXP_C=$(SRCDIR)lib/snolib/exp.c
FILE_C=$(SRCDIR)lib/snolib/file.c
FINDUNIT_C=$(SRCDIR)lib/snolib/findunit.c
GETSTRING_C=$(SRCDIR)lib/snolib/getstring.c
HANDLE_C=$(SRCDIR)lib/snolib/handle.c
HOST_C=$(SRCDIR)lib/snolib/host.c
LOG_C=$(SRCDIR)lib/snolib/log.c
LOGIC_C=$(SRCDIR)modules/logic/logic.c
NDBM_C=$(SRCDIR)modules/ndbm/ndbm.c
ORD_C=$(SRCDIR)lib/snolib/ord.c
PTY_C=$(SRCDIR)lib/posix/pty.c
RANDOM_C=$(SRCDIR)modules/random/random.c
READLINE_C=$(SRCDIR)modules/readline/readline.c
RENAME_C=$(SRCDIR)lib/snolib/rename.c
RETSTRING_C=$(SRCDIR)lib/snolib/retstring.c
SERV_C=$(SRCDIR)lib/snolib/serv.c
SIN_C=$(SRCDIR)lib/snolib/sin.c
SPRINTF_C=$(SRCDIR)modules/sprintf/sprintf.c
SQRT_C=$(SRCDIR)lib/snolib/sqrt.c
SSET_C=$(SRCDIR)lib/snolib/sset.c
STCL_C=$(SRCDIR)lib/snolib/stcl.c
SYS_C=$(SRCDIR)lib/posix/sys.c
TAN_C=$(SRCDIR)lib/snolib/tan.c
# for cygwin!
COM_CPP=$(SRCDIR)lib/win32/com.cpp

# private copy of CFLAGS for data_init.o; here so it can be overridden
# (ie; to just $(MYCPPFLAGS)) by config.m4 during debug (optimizing it
# is painful but worthwhile for production, or for compiler bugs!)

DATA_INIT_CFLAGS=$(CFLAGS)

################ objects

# or inet6.o
INET_O=	inet.o

# end of defaults
################################################################
CMT()
CMT([establish base values:])
ADD_OPT([$(OPT)])
CMT()
# config.m4:
include(config.m4)
# end of local config
################################################################

# for PML functions.
ADD_LDFLAGS([$(MATHLIB)])

# after local config

# NOTE: NOT named CPPFLAGS; some versions of make include CPPFLAGS in cc cmd
MYCPPFLAGS=-I$(SRCDIR)include -I$(SRCDIR). _CPPFLAGS

COPT=[]_OPT

LDFLAGS=[]_LDFLAGS

################
# compiler flags

CFLAGS=[]_CFLAGS $(COPT) $(MYCPPFLAGS) 

################

# add to new files to SRCS too!

# from configure ADD_OBJS:
AUX_OBJS=[]_OBJS

OBJS=	main.o $(SNOBOL4).o data.o data_init.o syn.o bal.o \
	break.o date.o dump.o dynamic.o endex.o expops.o \
	fisatty.o hash.o getstring.o handle.o $(INET_O) \
	init.o intspc.o io.o lexcmp.o load.o loadx.o \
	mstime.o ordvst.o pair.o pat.o pml.o ptyio_obj.o \
	realst.o replace.o retstring.o spcint.o spreal.o \
	stdio_obj.o str.o stream.o suspend.o term.o top.o \
	tree.o tty.o \
	atan.o chop.o cos.o delete.o environ.o execute.o exists.o \
	exit.o exp.o file.o findunit.o host.o ord.o log.o rename.o \
	serv.o sin.o sqrt.o sset.o sys.o tan.o \
	$(AUX_OBJS)

# from configure ADD_SRCS
AUX_SRCS=[]_SRCS

SRCS=	$(SRCDIR)main.c $(SRCDIR)$(SNOBOL4).c $(SRCDIR)data.c \
	$(SRCDIR)data_init.c $(SRCDIR)syn.c $(BAL_C) \
	$(BREAK_C) $(DATE_C) $(DUMP_C) $(DYNAMIC_C) \
	$(ENDEX_C) $(EXPOPS_C) $(FISATTY_C) $(GETSTRING_C) \
	$(HANDLE_C) $(HASH_C) $(INET_C) $(INIT_C) \
	$(INTSPC_C) $(IO_C) $(LEXCMP_C) $(LOAD_C) \
	$(MSTIME_C) $(ORDVST_C) $(PAIR_C) $(PAT_C) $(PML_C) \
	$(PTYIO_OBJ_C) $(REALST_C) $(REPLACE_C) \
	$(RETSTRING_C) $(SPCINT_C) $(SPREAL_C) \
	$(STDIO_OBJ_C) $(STR_C) $(STREAM_C) $(SUSPEND_C) \
	$(TERM_C) $(TOP_C) $(TREE_C) $(TTY_C) \
	$(ATAN_C) $(CHOP_C) $(COS_C) $(DELETE_C) $(ENVIRON_C) \
	$(EXISTS_C) $(EXIT_C) $(EXECUTE_C) $(EXP_C) $(FILE_C) \
	$(FINDUNIT_C) $(HOST_C) $(LOG_C) \
	$(LOGIC_C) $(ORD_C) $(RANDOM_C) $(RENAME_C) \
	$(SERV_C) $(SIN_C) $(SPRINTF_C) $(SQRT_C) \
	$(SSET_C) $(SYS_C) $(TAN_C) \
	$(AUX_SRCS)

GENERATED_DOCS_DOCDIR1=doc/sdb.1 doc/snobol4.1 doc/snopea.1 \
	doc/snobol4blocks.1 doc/snobol4cmd.1 doc/snobol4ctrl.1 \
	doc/snobol4error.1 doc/snobol4ext.1 doc/snobol4func.1 \
	doc/snobol4io.1 doc/snobol4key.1 doc/snobol4op.1

GENERATED_DOCS_DOCDIR3=doc/snobol4host.3 \
	doc/snobol4setup.3 doc/snolib.3 doc/snobol4ezio.3

GENERATED_DOCS_DOCDIR7=doc/snopea.7

GENERATED_DOCS_DOCDIR=$(GENERATED_DOCS_DOCDIR1) \
	$(GENERATED_DOCS_DOCDIR3) \
	$(GENERATED_DOCS_DOCDIR7)

GENERATED_DOCS=	$(GENERATED_DOCS_DOCDIR)

################
# link, regression test & timing

.PRECIOUS: $(SNOBOL4).o data_init.o snobol4

BUILD_ALL=sdb snobol4 snopea docs
build_all: $(BUILD_ALL)


xsnobol4: $(OBJS)
	rm -f xsnobol4$(EXT)
	$(MAKE) -f Makefile2 build.o
	$(CC) -o xsnobol4 $(OBJS) build.o $(LDFLAGS) $(SNOBOL4_LDFLAGS)

changequote(@,@)dnl

# invoked in submake by xsnobol4, $(SOFILENAME) rules
build.o: always
	rm -f build.c
	echo '/* MACHINE GENERATED.  EDITING IS FUTILE */'	> build.c
	echo '#include "h.h"'					>> build.c
	echo 'const char build_files[] = "'$(SRCS)'";'		>> build.c
	echo 'const char build_date[] = "'`date`'";'		>> build.c
	echo 'const char build_dir[] = "'`pwd`'";'		>> build.c
	$(CC) $(CFLAGS) -c build.c
changequote([,])dnl

always:


# avoid CFLAGS: -O causes crash on gcc 4.4.6 x86_64?
cpuid:	cpuid.c
	$(CC) -o cpuid cpuid.c

################ shared library

SO=so
SONAME=snobol4
SOFILENAME=lib$(SONAME)$(SO_EXT)

# do actual work to make shared library.
# invoked by top level Makefile
# in so directory, running so/Makefile2 (created with custom config.m4)
# with target shared_library and SRCDIR=../

# for Makefile, so it is agnostic about filename/extension:
shared_library: $(SOFILENAME)

$(SOFILENAME): $(OBJS)
	$(MAKE) -f Makefile2 build.o
	$(SO_LD) -o $(SOFILENAME) $(SO_LDFLAGS) $(OBJS) build.o $(LDFLAGS)

#### make shared library for ssnobol4 rule (below)

# for ssnobol4, from here (Makefile2), in top directory
# ask top level Makefile to build so/Makefile2 and run
# to make shared library.

$(SO)/$(SOFILENAME): always
	$(MAKE) shared_library

#### snobol4 executable using shared library

# invoked from Makefile, in top directory
# ask top level Makefile to build so/Makefile2 and run it

# unless $(SOFILENAME) is installed a well-known location
# you'll likely need to set LD_LIBRARY_PATH
# (or DYLD_LIBRARY path on OSX)

# snobol4 interpreter using shared library (may be up to 8% slower)
ssnobol4: $(SO)/$(SOFILENAME) ssnobol4.c
	$(CC) -Iinclude -o ssnobol4 ssnobol4.c -L$(SO) -l$(SONAME)

# tiny test of invoking SNOBOL4 in a program:
tlib: $(SO)/$(SOFILENAME) tlib.c
	$(CC) -Iinclude -o tlib tlib.c -L$(SO) -l$(SONAME)

################
# run regression tests.

timing.out: tested xsnobol4 timing timing.sno test/bench.sno test/v311.sil
	@echo Running timing script...
	./timing > timing.out.tmp
	mv timing.out.tmp timing.out
	@echo '********************************************************' 1>&2
	@echo 'Please consider mailing timing.out to timing@snobol4.org' 1>&2
	@echo 'Anonymized results are posted at http://www.snobol4.org' 1>&2
	@echo 'And you will be notified when test versions are available.' 1>&2
	@echo '********************************************************' 1>&2

tested snobol4: xsnobol4 test/tests.in cpuid $(MODULES_GENERATED)
	@echo Running regression tests...
	(cd test; BLOCKS=$(BLOCKS) SNOPATH="$(TEST_SNOPATH)" ./run.sh ../xsnobol4 -N)
	@echo Passed regression tests.
	-rm -f snobol4$(EXT)
	cp xsnobol4$(EXT) snobol4$(EXT)
	date > tested

################

# may need additional options due to size (and SIL code issues)
$(SNOBOL4).o: $(SRCDIR)$(SNOBOL4).c 
	$(CC) $(CFLAGS) $(SNOBOL4_C_CFLAGS) -c $(SRCDIR)$(SNOBOL4).c

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
# NOTE! 9/2013: gcc no longer takes forever!!
data_init.o: $(SRCDIR)data_init.c 
	$(CC) $(DATA_INIT_CFLAGS) -c $(SRCDIR)data_init.c

syn.o: $(SRCDIR)syn.c
	$(CC) $(CFLAGS) -c $(SRCDIR)syn.c

################ scripts

sdb:	sdb.sh config.m4
	rm -f sdb
	sed -e "s@<SNOLIB_LIB>@$(SNOLIB_LIB)@" \
	    -e "s@<BINDIR>@$(BINDIR)@" \
	    -e "s@<VERS>@-$(VERS)@" \
	    < sdb.sh > sdb
	chmod a+x sdb

snopea:	snopea.in config.m4
	rm -f snopea
	sed -e "s@<BINDIR>@$(BINDIR)@" \
	    -e "s@<VERS>@-$(VERS)@" \
	    < snopea.in > snopea
	chmod a+x snopea

#################################################################
# lib files

bal.o:	$(BAL_C)
	$(CC) $(CFLAGS) -c $(BAL_C)

break.o: $(BREAK_C)
	$(CC) $(CFLAGS) -c $(BREAK_C)

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

fisatty.o: $(FISATTY_C)
	$(CC) $(CFLAGS) -c $(FISATTY_C)

hash.o:	$(HASH_C)
	$(CC) $(CFLAGS) -c $(HASH_C)

inet.o:	$(INET_C)
	$(CC) $(CFLAGS) -c $(INET_C)

inet6.o: $(INET6_C)
	$(CC) $(CFLAGS) -c $(INET6_C)

init.o:	$(INIT_C)
	$(CC) $(CFLAGS) -c $(INIT_C)

intspc.o: $(INTSPC_C)
	$(CC) $(CFLAGS) -c $(INTSPC_C)

io.o:	$(IO_C) $(MAKEFILE2)
	$(CC) $(CFLAGS) -c $(IO_C)

lexcmp.o: $(LEXCMP_C)
	$(CC) $(CFLAGS) -c $(LEXCMP_C)

load.o:	$(LOAD_C) $(MAKEFILE2)
	$(CC) $(CFLAGS) -c $(LOAD_C)

loadx.o: $(LOADX_C)
	$(CC) $(CFLAGS) -c $(LOADX_C)

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

popen.o: $(POPEN_C)
	$(CC) $(CFLAGS) -c $(POPEN_C)

realst.o: $(REALST_C)
	$(CC) $(CFLAGS) -c $(REALST_C)

replace.o: $(REPLACE_C)
	$(CC) $(CFLAGS) -c $(REPLACE_C)

sleep.o: modules/time/$(SLEEP_C)
	$(CC) $(CFLAGS) -c modules/time/$(SLEEP_C)

spcint.o: $(SPCINT_C)
	$(CC) $(CFLAGS) -c $(SPCINT_C)

spreal.o: $(SPREAL_C)
	$(CC) $(CFLAGS) -c $(SPREAL_C)

stdio_obj.o: $(STDIO_OBJ_C)
	$(CC) $(CFLAGS) -c $(STDIO_OBJ_C)

str.o: $(STR_C)
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

ptyio_obj.o: $(PTYIO_OBJ_C)
	$(CC) $(CFLAGS) -c $(PTYIO_OBJ_C)

#################
# aux files -- porting aids not used in all builds;

bzero.o: $(BZERO_C)
	$(CC) $(CFLAGS) -c $(BZERO_C)

bcopy.o: $(BCOPY_C)
	$(CC) $(CFLAGS) -c $(BCOPY_C)

getopt.o: $(GETOPT_C)
	$(CC) $(CFLAGS) -c $(GETOPT_C)

bindresvport.o: $(BINDRESVPORT_C)
	$(CC) $(CFLAGS) -c $(BINDRESVPORT_C)

CLOSEFROM_C=$(SRCDIR)lib/bsd/closefrom.c
closefrom.o: $(CLOSEFROM_C)
	$(CC) $(CFLAGS) -c $(CLOSEFROM_C)

getdtablesize.o: $(GETDTABLESIZE_C)
	$(CC) $(CFLAGS) -c $(GETDTABLESIZE_C)

getline.o: $(GETLINE_C)
	$(CC) $(CFLAGS) -c $(GETLINE_C)

bufio_obj.o: $(BUFIO_OBJ_C)
	$(CC) $(CFLAGS) -c $(BUFIO_OBJ_C)

memio_obj.o: $(MEMIO_OBJ_C)
	$(CC) $(CFLAGS) -c $(MEMIO_OBJ_C)

################
# SDBM files -- only when needed

sdbm.o: $(SDBM_C)
	$(CC) $(CFLAGS) -c $(SDBM_C)

sdbm_hash.o: $(SDBM_HASH_C)
	$(CC) $(CFLAGS) -c $(SDBM_HASH_C)

sdbm_pair.o: $(SDBM_PAIR_C)
	$(CC) $(CFLAGS) -c $(SDBM_PAIR_C)

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
# Used to be snolib.a
# "external" functions that are always built-in via pml.h
# everything else is now built as a module!

atan.o:	$(ATAN_C)
	$(CC) $(CFLAGS) -c $(ATAN_C)

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

handle.o: $(HANDLE_C)
	$(CC) $(CFLAGS) -c $(HANDLE_C)

HOST_C_CFLAGS=-DCC=\""$(CC)"\" -DCOPT=\""$(COPT)"\" -DHAVE_BUILD_VARS
host.o: $(HOST_C)
	$(CC) $(CFLAGS) $(HOST_C_CFLAGS) -c $(HOST_C)

log.o: $(LOG_C)
	$(CC) $(CFLAGS) -c $(LOG_C)

logic.o: $(LOGIC_C)
	$(CC) $(CFLAGS) -c $(LOGIC_C)

ndbm.o: $(NDBM_C)
	$(CC) $(CFLAGS) -c $(NDBM_C)

ord.o: $(ORD_C)
	$(CC) $(CFLAGS) -c $(ORD_C)

pty.o:	$(PTY_C)
	$(CC) $(CFLAGS) -c $(PTY_C)

random.o: $(RANDOM_C)
	$(CC) $(CFLAGS) -c $(RANDOM_C)

readline.o: $(READLINE_C)
	$(CC) $(CFLAGS) -c $(READLINE_C)

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

stcl.o: $(STCL_C)
	$(CC) $(CFLAGS) -c $(STCL_C)

sys.o:	$(SYS_C)
	$(CC) $(CFLAGS) -c $(SYS_C)

tan.o:	$(TAN_C)
	$(CC) $(CFLAGS) -c $(TAN_C)

time.o:	$(TIME_C)
	$(CC) $(CFLAGS) -c $(TIME_C)

com.o:	$(COM_CPP)
	$(CC) $(CFLAGS) -c $(COM_CPP)

#################
# lint picking

lint:   llib-lf.ln
	lint -h llib-lf.ln $(MYCPPFLAGS) $(SRCS) > LINT 2>&1

llib-lf.ln:
	lint -Cf $(MYCPPFLAGS) $(SRCS) > /dev/null 2>&1

################
# GENERATED_DOCS:

SNOPEA=./snobol4 -N -Isnolib -I. snopea.in

snopea.1: snopea snolib/snopea.sno snobol4
	$(SNOPEA) snopea snopea.1

snopea.1.html: snopea snolib/snopea.sno snobol4
	$(SNOPEA) snopea snopea.1.html

docs $(GENERATED_DOCS_DOCDIR): snopea snolib/snopea.sno snobol4 always
	cd doc; make

always:

#################
# installation

# install .h files for dynamicly loaded functions
INSTALL_H=[include]/h.h [include]/snotypes.h [include]/macros.h \
	[include]/load.h [include]/dt.h [include]/str.h [include]/handle.h \
	config.h equ.h version.h

# generated SNOLIB files (host.sno generated at top level)
GENSNOLIB=host.sno config.sno

SNOLIB_FILES=snolib/*.sno $(GENSNOLIB) $(MODULES_INCLUDE)

install: snobol4 sdb timing.out $(GENERATED_DOCS)
	$(INSTALL) -d $(BINDIR)
	$(INSTALL) $(INSTALL_BIN_FLAGS) snobol4 $(BINDIR)/snobol4-$(VERS)
	$(INSTALL) sdb $(BINDIR)/sdb-$(VERS)
	$(INSTALL) snopea $(BINDIR)/snopea-$(VERS)
	rm -f $(BINDIR)/snobol4 $(BINDIR)/sdb $(BINDIR)/snopea
	ln -s $(BINDIR)/snobol4-$(VERS) $(BINDIR)/snobol4
	ln -s $(BINDIR)/sdb-$(VERS) $(BINDIR)/sdb
	ln -s $(BINDIR)/snopea-$(VERS) $(BINDIR)/snopea
	$(INSTALL) -d $(MAN1DIR)
	for F in $(GENERATED_DOCS_DOCDIR1); do \
		$(INSTALL) -m 644 $$F $(MAN1DIR); \
	done
	$(INSTALL) -d $(MAN3DIR)
	for F in $(GENERATED_DOCS_DOCDIR3); do \
		$(INSTALL) -m 644 $$F $(MAN3DIR); \
	done
	$(INSTALL) -d $(MAN7DIR)
	for F in $(GENERATED_DOCS_DOCDIR7); do \
		$(INSTALL) -m 644 $$F $(MAN7DIR); \
	done
	$(INSTALL) -d $(SNOLIB)
	$(INSTALL) -d $(SNOLIB_DOC)
	$(INSTALL) -m 644 README $(SNOLIB_DOC)
	$(INSTALL) -m 644 CHANGES $(SNOLIB_DOC)
	$(INSTALL) -d $(SNOLIB)/local
	$(INSTALL) -d $(SNOLIB_LIB)
	$(INSTALL) -d $(SNOLIB_LIB)/shared
	$(INSTALL) -d $(SNOLIB_LOCAL)
	$(INSTALL) -d $(SNOLIB_LOCAL)/shared
	rm -f $(SNOLIB_LIB)/dynload
	rm -f $(MAN3DIR)/snobol4tcl.3 $(MAN3DIR)/snobol4dbm.3
	for F in $(SNOLIB_FILES); do \
		$(INSTALL) -m 644 $$F $(SNOLIB_LIB); \
	done
	for M in $(MODULES); do \
		(cd modules/$$M; $(RUNSETUP) install); \
	done
	$(INSTALL) -d $(INCLUDE_DIR)
	for F in $(INSTALL_H); do \
		$(INSTALL) -m 644 $$F $(INCLUDE_DIR); \
	done
	$(INSTALL) -d $(DOC_DIR)
	for F in doc/load.txt doc/*.html modules/*/*.html; do \
		$(INSTALL) -m 644 $$F $(DOC_DIR); \
	done
ifdef([INSTALL_SYSDEP],[	]INSTALL_SYSDEP
,)dnl
	@echo '*********************************************************' 1>&2
	@echo 'Have you mailed a copy of timing.out to timing@snobol4.org ?' 1>&2
	@echo '*********************************************************' 1>&2

printenv:
	env

################
MAKEFILE2=Makefile2

# was MYCPPFLAGS, changed to pick up -DSHARED
DEPENDFLAGS=$(CFLAGS)

depend:
	sed '/^# DO NOT DELETE THIS LINE/q' $(MAKEFILE2) > $(MAKEFILE2).tmp
	$(CCM) $(DEPENDFLAGS) $(SRCS) >> $(MAKEFILE2).tmp
	mv -f $(MAKEFILE2).tmp $(MAKEFILE2)
