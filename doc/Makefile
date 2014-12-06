# Makefile for documentation
# $Id$

GENERATED1=sdb.1 snobol4error.1
GENERATED7=snopea.7

GENERATED=$(GENERATED1) $(GENERATED7)

ALL=	$(GENERATED) $(TXT) $(HTML)

TXT=	porting.txt

HTML=	snobol4.1.html sdb.1.html \
	snobol4error.1.html \
	snobol4readline.3.html \
	snopea.7.html

PS=	snobol4.1.ps sdb.1.ps \
	snobol4error.1.ps \
	snobol4readline.3.ps \
	snopea.7.ps

PDF=	snobol4.1.pdf sdb.1.pdf \
	snobol4error.1.pdf \
	snobol4readline.3.pdf \
	snopea.7.pdf

all:	$(ALL)
txt:	$(TXT)
html:	$(GENERATED) $(HTML)
ps:	$(GENERATED) $(PS)
pdf:	$(GENERATED) $(PDF)

$(GENERATED): ../snobol4

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
SNOPEA_DEPS=../snobol4 ../snopea ../snolib/snopea.sno

.SUFFIXES: .html .ms .txt .ps .pdf

####

.ms.txt:
	$(NROFF) $(MS) $< > $@

.ms.html:
	$(TROFF_HTML) $(MS) $< > $@

.ms.ps:
	$(TROFF_PS) $(MS) $< > $@

################
# explicit rules

HHH=--format html

snobol4.1.html: snobol4.1
	$(TROFF_HTML) snobol4.1 > snobol4.1.html

snobol4readline.3.html: snobol4readline.3
	$(TROFF_HTML) snobol4readline.3 > snobol4readline.3.html

snobol4error.1: snobol4error.1.pea $(SNOPEA_DEPS)
	$(SNOPEA) snobol4error.1.pea snobol4error.1

snobol4error.1.html: snobol4error.1.pea $(SNOPEA_DEPS)
	$(SNOPEA) $(HHH) snobol4error.1.pea snobol4error.1.html

sdb.1: sdb.1.pea $(SNOPEA_DEPS)
	$(SNOPEA) sdb.1.pea sdb.1

sdb.1.html: sdb.1.pea $(SNOPEA_DEPS)
	$(SNOPEA) $(HHH) sdb.1.pea sdb.1.html

snolib.html: snolib.3
	$(TROFF_HTML) snolib.3 > snolib.3.html

snopea.7: snopea.7.pea $(SNOPEA_DEPS)
	$(SNOPEA) snopea.7.pea snopea.7

snopea.7.html: snopea.7.pea $(SNOPEA_DEPS)
	$(SNOPEA) $(HHH) snopea.7.pea snopea.7.html

####

clean:
	rm -f porting.txt *.html *.ps *.pdf \#* *~ $(GENERATED)
