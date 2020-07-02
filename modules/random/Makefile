MOD=random

all:	$(MOD).sno $(MOD).so

# setuputil.sno, host.sno
INC=-I../../snolib -I../..
SNOBOL4=../../snobol4
SETUP=$(SNOBOL4) $(INC) setup.sno

$(MOD).sno $(MOD).so: setup.sno random.c
	$(SETUP) build

install:
	$(SETUP) install

clean:
	$(SETUP) clean
