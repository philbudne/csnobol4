# Makefile for doc files
# $Id$

GENERATED=sdb.1 snopea.7 snopea-format.7

ALL=	$(GENERATED)

TXT=	porting.txt snobol4.0 sdb.0 \
	snobol4readline.0 \
	snopea-format.0

HTML=	snobol4.html sdb.html \
	snobol4readline.html \
	snopea-format.html

PS=	snobol4.ps sdb.ps \
	snobol4readline.ps \
	snopea-format.ps

PDF=	snobol4.pdf sdb.pdf \
	snobol4readline.pdf \
	snopea-format.pdf

all:	$(ALL)
txt:	$(GENERATED) $(TXT)
html:	$(GENERATED) $(HTML)
ps:	$(GENERATED) $(PS)
pdf:	$(GENERATED) $(PDF)

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

SNOPEA=../xsnobol4 -N -I../snolib -I.. ../snopea

.SUFFIXES: .ms .1 .3 .7 .0 .txt .ps .pdf .html .pea

####

.ms.txt:
	$(NROFF) $(MS) $< > $@

.ms.html:
	$(TROFF_HTML) $(MS) $< > $@

.ms.ps:
	$(TROFF_PS) $(MS) $< > $@

.pea.html:
	$(SNOPEA) --format html $< $@

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
.pea.1:
	$(SNOPEA) $< $@

.pea.1.html:
	$(SNOPEA) --format html $< $@

####

.pea.7:
	$(SNOPEA) $< $@

################

snopea-format.html: snopea.pea
	$(SNOPEA) --format html --section 7 snopea.pea snopea-format.html

snopea-format.7:
	ln -s snopea.7 snopea-format.7

####

clean:
	rm -f *.0 porting.txt *.html *.ps *.pdf \#* *~ $(GENERATED)
