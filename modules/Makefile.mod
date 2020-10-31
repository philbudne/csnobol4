# $Id$
# invoked from module/X/Makefiles as ../Makefile.mod

# may be overriden on command line
SRC=$(MOD).c

# binary output extension may differ (so, dylib, dll)
OUT=$(MOD).sno

all:	$(OUT)

INC=-N -I../.. -I../../snolib
SNOBOL4?=../../xsnobol4
SETUP=$(SNOBOL4) $(INC) setup.sno $(SETUPOPT) ../../config.sno

$(OUT): setup.sno $(SRC) ../../snolib/setuputil.sno $(SNOBOL4)
	$(SETUP) build

test:	$(OUT)
	$(SETUP) test

install: $(OUT)
	$(SETUP) install

clean:
	$(SETUP) clean
	rm -f *~
