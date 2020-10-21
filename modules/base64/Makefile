MOD=base64

all:	$(MOD).sno $(MOD).so

INC=-N -I../.. -I../../snolib
SNOBOL4=../../snobol4
SETUP=$(SNOBOL4) $(INC) setup.sno $(V)
#V=-v

$(MOD).sno $(MOD).so: setup.sno $(MOD).c funcs.sno
	$(SETUP) build

install:
	$(SETUP) install

test:	$(MOD).sno $(MOD).so
	$(SNOBOL4) $(INC) test.sno

clean:
	$(SETUP) clean
