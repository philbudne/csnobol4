# Makefile for doc files
# $Id$

ALL=	txt html

TXT=	porting.txt snobol4.0 sdb.0 \
	snobol4readline.0 \
	snopea.0

HTML=	snobol4.html sdb.html \
	snobol4readline.html \
	snopea.7.html

PS=	snobol4.ps sdb.ps \
	snobol4readline.ps \
	snopea.7.ps

PDF=	snobol4.pdf sdb.pdf \
	snobol4readline.pdf \
	snopea.7.pdf

all:	$(ALL)
txt:	$(TXT)
html:	$(HTML)
ps:	$(PS)
pdf:	$(PDF)

test:	test.7 test.0 test.7.html test.ps

MS=-ms
MAN=-man

# Use groff; does bolding!!
NROFF=nroff -Tascii -c

# troff to PostScript
TROFF_PS=groff -Tps

# troff to HTML
TROFF_HTML=groff -Thtml

# PostScript to PSF
PS2PDF=ps2pdf

.SUFFIXES: .ms .1 .3 .7 .0 .txt .ps .pdf .7.html .html .pea

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
.7.0:
	$(NROFF) $(MAN) $< > $@

.7.ps:
	$(TROFF_PS) $(MAN) $< > $@

####
.ps.pdf:
	$(PS2PDF) $< $@

####
.pea.7:
	../xsnobol4 -I../snolib ../snopea $< $@

.pea.7.html:
	../xsnobol4 -I../snolib ../snopea --format html $< $@

####

clean:
	rm -f *.0 porting.txt *.html *.ps *.pdf \#* *~ snopea.7 test.7
