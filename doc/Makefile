# Makefile for doc files
# $Id$

ALL=	txt html ps pdf
TXT=	porting.txt snobol4.0 sdb.0 \
	snobol4dbm.0 snobol4tcl.0 snobol4time.0
HTML=	porting.html snobol4.html sdb.html \
	snobol4dbm.html snobol4tcl.html snobol4time.html
PS=	porting.ps snobol4.ps sdb.ps \
	snobol4dbm.ps snobol4tcl.ps snobol4time.ps
PDF=	porting.pdf snobol4.pdf sdb.pdf \
	snobol4dbm.pdf snobol4tcl.pdf snobol4time.pdf

all:	$(ALL)
txt:	$(TXT)
html:	$(HTML)
ps:	$(PS)
pdf:	$(PDF)

MS=-ms
MAN=-man

# Use groff; does bolding!!
NROFF=groff -Tascii

# troff to PostScript
TROFF_PS=groff -Tps

# troff to HTML
TROFF_HTML=groff -Thtml

# PostScript to PSF
PS2PDF=ps2pdf

####

porting.txt: porting.ms Makefile
	$(NROFF) $(MS) porting.ms > porting.txt

porting.html: porting.ms Makefile
	$(TROFF_HTML) $(MS) porting.ms > porting.html

porting.ps: porting.ms Makefile
	$(TROFF_PS) $(MS) porting.ms > porting.ps

porting.pdf: porting.ps
	$(PS2PDF) porting.ps porting.pdf

####

snobol4.0: snobol4.1 Makefile
	$(NROFF) $(MAN) snobol4.1 > snobol4.0

snobol4.html: snobol4.1 Makefile
	$(TROFF_HTML) $(MAN) snobol4.1 > snobol4.html

snobol4.ps: snobol4.1 Makefile
	$(TROFF_PS) $(MAN) snobol4.1 > snobol4.ps

snobol4.pdf: snobol4.ps
	$(PS2PDF) snobol4.ps snobol4.pdf

####

sdb.0: sdb.1 Makefile
	$(NROFF) $(MAN) sdb.1 > sdb.0

sdb.html: sdb.1 Makefile
	$(TROFF_HTML) $(MAN) sdb.1 > sdb.html

sdb.ps: sdb.1 Makefile
	$(TROFF_PS) $(MAN) sdb.1 > sdb.ps

sdb.pdf: sdb.ps
	$(PS2PDF) sdb.ps sdb.pdf

####

snolib.0: snolib.3 Makefile
	$(NROFF) $(MAN) snolib.3 > snolib.0

snolib.html: snolib.3 Makefile
	$(TROFF_HTML) $(MAN) snolib.3 > snolib.html

snolib.ps: snolib.3 Makefile
	$(TROFF_PS) $(MAN) snolib.3 > snolib.ps

snolib.pdf: snolib.ps
	$(PS2PDF) snolib.ps snolib.pdf

####

snobol4dbm.0: snobol4dbm.3 Makefile
	$(NROFF) $(MAN) snobol4dbm.3 > snobol4dbm.0

snobol4dbm.html: snobol4dbm.3 Makefile
	$(TROFF_HTML) $(MAN) snobol4dbm.3 > snobol4dbm.html

snobol4dbm.ps: snobol4dbm.3 Makefile
	$(TROFF_PS) $(MAN) snobol4dbm.3 > snobol4dbm.ps

snobol4dbm.pdf: snobol4dbm.ps
	$(PS2PDF) snobol4dbm.ps snobol4dbm.pdf

####

snobol4tcl.0: snobol4tcl.3 Makefile
	$(NROFF) $(MAN) snobol4tcl.3 > snobol4tcl.0

snobol4tcl.html: snobol4tcl.3 Makefile
	$(TROFF_HTML) $(MAN) snobol4tcl.3 > snobol4tcl.html

snobol4tcl.ps: snobol4tcl.3 Makefile
	$(TROFF_PS) $(MAN) snobol4tcl.3 > snobol4tcl.ps

snobol4tcl.pdf: snobol4tcl.ps
	$(PS2PDF) snobol4tcl.ps snobol4tcl.pdf

####

snobol4time.0: snobol4time.3 Makefile
	$(NROFF) $(MAN) snobol4time.3 > snobol4time.0

snobol4time.html: snobol4time.3 Makefile
	$(TROFF_HTML) $(MAN) snobol4time.3 > snobol4time.html

snobol4time.ps: snobol4time.3 Makefile
	$(TROFF_PS) $(MAN) snobol4time.3 > snobol4time.ps

snobol4time.pdf: snobol4time.ps
	$(PS2PDF) snobol4time.ps snobol4time.pdf

################

clean:
	rm -f $(ALL) $(PORTING)
