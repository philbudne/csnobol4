# Makefile for doc files
# $Id$

ALL=	txt html ps pdf

TXT=	porting.txt snobol4.0 sdb.0 \
	snobol4dbm.0 snobol4random.0 snobol4readline.0 \
	snobol4tcl.0 snobol4time.0

HTML=	porting.html snobol4.html sdb.html \
	snobol4dbm.html snobol4random.html snobol4readline.html \
	snobol4tcl.html snobol4time.html

PS=	porting.ps snobol4.ps sdb.ps \
	snobol4dbm.ps snobol4random.ps snobol4readline.ps \
	snobol4tcl.ps snobol4time.ps

PDF=	porting.pdf snobol4.pdf sdb.pdf \
	snobol4dbm.pdf snobol4random.pdf snobol4readline.pdf \
	snobol4tcl.pdf snobol4time.pdf

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

.SUFFIXES: .ms .1 .3 .0 .txt .ps .pdf .html

####

.ms.txt:
	$(NROFF) $(MS) $< > $@

.ms.html:
	$(TROFF_HTML) $(MS) $< > $@

.ms.ps:
	$(TROFF_PS) $(MS) $< > $@

####
.1.0:
	$(NROFF) $(MAN) $< > $@

.1.html:
	$(TROFF_HTML) $(MAN) $< > $@

.1.ps:
	$(TROFF_PS) $(MAN) $< > $@

####
.3.0:
	$(NROFF) $(MAN) $< > $@

.3.html:
	$(TROFF_HTML) $(MAN) $< > $@

.3.ps:
	$(TROFF_PS) $(MAN) $< > $@

####
.ps.pdf:
	$(PS2PDF) $< $@

####
clean:
	rm -f *.0 porting.txt *.html *.ps *.pdf \#* *~
