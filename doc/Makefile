# Makefile for doc files
# $Id$

all:	porting.doc porting.html snobol4.0 snobol4.html snobol4.ps

MS=-ms
MAN=-man

# Use groff; does bolding!!
NROFF=groff -Tascii

# troff to PostScript
TROFF_PS=groff -Tps

TROFF_HTML=groff -Thtml

porting.doc: porting.ms Makefile
	$(NROFF) $(MS) porting.ms > porting.doc

porting.html: porting.ms Makefile
	$(TROFF_HTML) $(MS) porting.ms > porting.html

porting.ps: porting.ms Makefile
	$(TROFF_PS) $(MS) porting.ms > porting.ps

snobol4.0: snobol4.1 Makefile
	$(NROFF) $(MAN) snobol4.1 > snobol4.0

snobol4.html: snobol4.1 Makefile
	$(TROFF_HTML) $(MAN) snobol4.1 > snobol4.html

snobol4.ps: snobol4.1 Makefile
	$(TROFF_PS) $(MAN) snobol4.1 > snobol4.ps
