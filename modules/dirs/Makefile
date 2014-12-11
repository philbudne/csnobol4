MOD=dirs

all:	$(MOD).sno $(MOD).so

INC=-I..
SNOBOL4=../../snobol4
SETUP=$(SNOBOL4) $(INC) setup.sno

$(MOD).sno $(MOD).so: setup.sno $(MOD).c
	$(SETUP) build

install:
	$(SETUP) install

clean:
	$(SETUP) clean
