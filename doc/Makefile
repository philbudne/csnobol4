# Makefile for doc files
# $Id$

all:	porting.doc snobol4.0

## try also -Tcrt -Tman
# SunOS5/SVR4;
#NROFF=nroff -Tlp
# SunOS4;
NROFF=nroff -Tlpr

MS=-ms
MAN=-man

porting.doc: porting.ms
	$(NROFF) $(MS) porting.ms > porting.doc

snobol4.0: snobol4.1
	$(NROFF) $(MAN) snobol4.1 > snobol4.0


