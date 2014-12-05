# Makefile for documentation
# $Id$

GENERATED=sdb.1 snopea.7 snobol4error.1

ALL=	$(GENERATED) $(TXT) $(HTML)

TXT=	porting.txt \
	snobol4.0 sdb.0 \
	snobol4readline.0 \
	snobol4error.0 \
	snopea-format.0

HTML=	snobol4.html sdb.html \
	snobol4readline.html \
	snobol4error.html \
	snopea-format.html

PS=	snobol4.ps sdb.ps \
	snobol4readline.ps \
	snobol4error.ps \
	snopea-format.ps

PDF=	snobol4.pdf sdb.pdf \
	snobol4readline.pdf \
	snobol4error.pdf \
	snopea-format.pdf

all:	$(ALL)
txt:	$(GENERATED) $(TXT)
html:	$(GENERATED) $(HTML)
ps:	$(GENERATED) $(PS)
pdf:	$(GENERATED) $(PDF)

MS=-ms
MAN=-man

NROFF=nroff -Tascii -c

# troff to PostScript
TROFF_PS=troff -Tps

# troff to HTML
TROFF_HTML=troff -Thtml

# PostScript to PSF
PS2PDF=ps2pdf

# run snopea using our own files
SNOPEA=../snobol4 -N -I../snolib -I.. ../snopea

.SUFFIXES: .ms .1 .3 .7 .0 .txt .ps .pdf .html .pea

####

.ms.txt:
	$(NROFF) $(MS) $< > $@

.ms.html:
	$(TROFF_HTML) $(MS) $< > $@

.ms.ps:
	$(TROFF_PS) $(MS) $< > $@

.pea.html:
	$(SNOPEA) --format html --section 1 $< $@

####

.pea.1:
	$(SNOPEA) $< $@

.pea.7:
	$(SNOPEA) $< $@

####
.1.0:
	$(NROFF) $(MAN) $< > $@

.1.ps:
	$(TROFF_PS) $(MAN) $< > $@

####
.3.0:
	$(NROFF) $(MAN) $< > $@

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

################
# explicit rules

snobol4.html: snobol4.1
	$(TROFF_HTML) snobol4.1 > snobol4.html

snobol4readline.html: snobol4readline.3
	$(TROFF_HTML) snobol4readline.3 > snobol4readline.html

snolib.html: snolib.3
	$(TROFF_HTML) snolib.3 > snolib.html

snopea-format.html: snopea.pea
	$(SNOPEA) --format html --section 7 snopea.pea snopea-format.html

snopea-format.0: snopea.7
	$(NROFF) -man snopea.7 > snopea-format.0

####

clean:
	rm -f *.0 porting.txt *.html *.ps *.pdf \#* *~ $(GENERATED)
