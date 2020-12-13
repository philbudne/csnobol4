# $Id$
# invoked from module/X/Makefiles as "make -f ../Makefile.mod MOD=modname ..."

# may be overriden on command line; may be multiple files, incl. funcs.sno
SRC=$(MOD).c

# binary output extension may differ (.so, .dylib, .dll)
OUT=$(MOD).sno

all:	$(OUT)

# not supported in Solaris make:
SNOBOL4?=snobol4

SETUP=$(SNOBOL4) setup.sno $(SETUPOPT)

# removed ../../config.sno and ../../host.sno
# to allow FreeBSD ports for individual modules
IN=setup.sno $(SRC) ../../snolib/setuputil.sno
$(OUT):	$(IN)
	$(SETUP) build

debug:	$(IN)
	$(SETUP) -d -v build

test:	$(OUT)
	$(SETUP) test

test_verbose: $(OUT)
	$(SETUP) -v test

install: $(OUT)
	$(SETUP) install

clean:
	$(SETUP) clean
	rm -f *~
