all:	$(MOD).sno $(MOD).so

INC=-N -I../.. -I../../snolib
SNOBOL4=../../xsnobol4
SETUP=$(SNOBOL4) $(INC) setup.sno

all: $(MOD).so $(MOD).sno

SRC=$(MOD).c
$(MOD).sno $(MOD).so: setup.sno $(SRC) ../../snolib/setuputil.sno
	$(SETUP) build

test: $(MOD).sno $(MOD).so
	if [ -f test.sno ]; then $(SNOBOL4) $(INC) -I. test.sno; else true; fi

install: $(MOD).sno $(MOD).so
	$(SETUP) install

clean:
	$(SETUP) clean
	 rm -f *~
