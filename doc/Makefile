# Makefile for doc files
# $Id$

ALL=porting.txt snobol4.0 snobol4.html snobol4.ps snobol4.pdf
all:	$(ALL)

PORTING=porting.html porting.ps
porting: $(PORTING)

MS=-ms
MAN=-man

# Use groff; does bolding!!
NROFF=groff -Tascii

# troff to PostScript
TROFF_PS=groff -Tps

# troff to HTML
TROFF_HTML=groff -Thtml

porting.txt: porting.ms Makefile
	$(NROFF) $(MS) porting.ms > porting.txt

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

snobol4.pdf: snobol4.ps
	ps2pdf snobol4.ps snobol4.pdf

clean:
	rm -f $(ALL) $(PORTING)
