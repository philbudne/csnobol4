# Makefile for doc files
# $Id$

all:	porting.doc

## try also -Tcrt -Tman
NROFF=nroff -Tlpr
MS=-ms

porting.doc: porting.ms
	$(NROFF) $(MS) porting.ms > porting.doc

