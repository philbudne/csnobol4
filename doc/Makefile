# Makefile for doc files
# $Id$

all:	porting.doc porting.html snobol4.0 snobol4.html

## try also -Tcrt -Tman
# SunOS5/SVR4;
#NROFF=nroff -Tlp
# SunOS4;
NROFF=nroff -Tlpr

MS=-ms
MAN=-man

# from http://www.sis.port.ac.uk/~briggsjs/troff2html.tar
TROFF2HTML=troff2html

# from http://www.oac.uci.edu/indiv/ehood/man2html.html
# not as good as TROFF2HTML (but no -man support in the latter)
MAN2HTML=man2html


porting.doc: porting.ms
	$(NROFF) $(MS) porting.ms > porting.doc

porting.html: porting.ms
	$(TROFF2HTML) $(MS) porting.ms > porting.html

snobol4.0: snobol4.1
	$(NROFF) $(MAN) snobol4.1 > snobol4.0

snobol4.html: snobol4.0
	$(MAN2HTML) < snobol4.0 > snobol4.html
