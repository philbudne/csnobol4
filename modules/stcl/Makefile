MOD=stcl

INC=-I..
SNOBOL4=../../snobol4
SETUP=$(SNOBOL4) $(INC) setup.sno

all:	$(MOD).sno $(MOD).so

$(MOD).sno $(MOD).so: setup.sno $(MOD).c
	$(SETUP) build

install:
	$(SETUP) install

clean:
	$(SETUP) clean

