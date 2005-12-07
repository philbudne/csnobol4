# Makefile for doc files
# $Id$

ALL=	porting.txt \
	snobol4.0 snobol4.html snobol4.ps snobol4.pdf \
	snolib.0 snolib.html snolib.ps snolib.pdf \
	snobol4dbm.0 snobol4dbm.html snobol4dbm.ps snobol4dbm.pdf \
	snobol4tcl.0 snobol4tcl.html snobol4tcl.ps snobol4tcl.pdf

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

####

porting.txt: porting.ms Makefile
	$(NROFF) $(MS) porting.ms > porting.txt

porting.html: porting.ms Makefile
	$(TROFF_HTML) $(MS) porting.ms > porting.html

porting.ps: porting.ms Makefile
	$(TROFF_PS) $(MS) porting.ms > porting.ps

####

snobol4.0: snobol4.1 Makefile
	$(NROFF) $(MAN) snobol4.1 > snobol4.0

snobol4.html: snobol4.1 Makefile
	$(TROFF_HTML) $(MAN) snobol4.1 > snobol4.html

snobol4.ps: snobol4.1 Makefile
	$(TROFF_PS) $(MAN) snobol4.1 > snobol4.ps

snobol4.pdf: snobol4.ps
	ps2pdf snobol4.ps snobol4.pdf

####

snolib.0: snolib.3 Makefile
	$(NROFF) $(MAN) snolib.3 > snolib.0

snolib.html: snolib.3 Makefile
	$(TROFF_HTML) $(MAN) snolib.3 > snolib.html

snolib.ps: snolib.3 Makefile
	$(TROFF_PS) $(MAN) snolib.3 > snolib.ps

snolib.pdf: snolib.ps
	ps2pdf snolib.ps snolib.pdf

####

snobol4dbm.0: snobol4dbm.3 Makefile
	$(NROFF) $(MAN) snobol4dbm.3 > snobol4dbm.0

snobol4dbm.html: snobol4dbm.3 Makefile
	$(TROFF_HTML) $(MAN) snobol4dbm.3 > snobol4dbm.html

snobol4dbm.ps: snobol4dbm.3 Makefile
	$(TROFF_PS) $(MAN) snobol4dbm.3 > snobol4dbm.ps

snobol4dbm.pdf: snobol4dbm.ps
	ps2pdf snobol4dbm.ps snobol4dbm.pdf

####

snobol4tcl.0: snobol4tcl.3 Makefile
	$(NROFF) $(MAN) snobol4tcl.3 > snobol4tcl.0

snobol4tcl.html: snobol4tcl.3 Makefile
	$(TROFF_HTML) $(MAN) snobol4tcl.3 > snobol4tcl.html

snobol4tcl.ps: snobol4tcl.3 Makefile
	$(TROFF_PS) $(MAN) snobol4tcl.3 > snobol4tcl.ps

snobol4tcl.pdf: snobol4tcl.ps
	ps2pdf snobol4tcl.ps snobol4tcl.pdf

################

clean:
	rm -f $(ALL) $(PORTING)
