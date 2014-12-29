# Makefile for documentation
# $Id$

GENERATED1=sdb.1 snobol4.1 snobol4blocks.1 snobol4cmd.1 snobol4ctrl.1 \
	snobol4error.1 snobol4ext.1 snobol4func.1 snobol4io.1 \
	snobol4key.1 snobol4op.1 snopea.1
GENERATED3=snolib.3 snobol4setup.3
GENERATED7=snopea.7

GENERATED=$(GENERATED1) $(GENERATED3) $(GENERATED7)

ALL=	$(GENERATED) $(TXT) $(HTML)

TXT=	porting.txt

HTML=	snobol4.1.html sdb.1.html snopea.1.html \
	snobol4blocks.1.html \
	snobol4cmd.1.html \
	snobol4ctrl.1.html \
	snobol4error.1.html \
	snobol4ext.1.html \
	snobol4func.1.html \
	snobol4io.1.html \
	snobol4key.1.html \
	snobol4op.1.html \
	snolib.3.html \
	snobol4setup.3.html \
	snopea.7.html

PS=	snobol4.1.ps sdb.1.ps snopea.1.ps \
	snobol4blocks.1.ps \
	snobol4cmd.1.ps \
	snobol4ctrl.1.ps \
	snobol4error.1.ps \
	snobol4ext.1.ps \
	snobol4func.1.ps \
	snobol4io.1.ps \
	snobol4key.1.ps \
	snobol4op.1.ps \
	snolib.3.ps \
	snobol4setup.3.ps \
	snopea.7.ps

PDF=	snobol4.1.pdf sdb.1.pdf snopea.1.pdf \
	snobol4blocks.1.pdf \
	snobol4cmd.1.pdf \
	snobol4ctrl.1.pdf \
	snobol4error.1.pdf \
	snobol4ext.1.pdf \
	snobol4func.1.pdf \
	snobol4io.1.pdf \
	snobol4key.1.pdf \
	snobol4op.1.pdf \
	snolib.3.pdf \
	snobol4setup.3.pdf \
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
TROFF_PS=groff -Tps

# troff to HTML
TROFF_HTML=groff -Thtml

# PostScript to PDF
PS2PDF=ps2pdf

# run snopea using our own files
SNOPEA=../snobol4 -N -I../snolib -I.. ../snopea.in
SNOPEA_DEPS=../snobol4 ../snopea.in ../snolib/snopea.sno

.SUFFIXES: .html .ms .txt .ps .pdf

####

.ms.txt:
	$(NROFF) $(MS) $< > $@

.ms.html:
	$(TROFF_HTML) $(MS) $< > $@

.ms.ps:
	$(TROFF_PS) $(MS) $< > $@

.ps.pdf:
	$(PS2PDF) $< $@

################
# explicit rules

snobol4.1: snobol4.1.pea $(SNOPEA_DEPS)
	$(SNOPEA) snobol4.1.pea snobol4.1

snobol4.1.html: snobol4.1.pea $(SNOPEA_DEPS)
	$(SNOPEA) snobol4.1.pea snobol4.1.html

snobol4.1.ps: snobol4.1
	$(TROFF_PS) -man snobol4.1 > snobol4.1.ps

####

snobol4blocks.1: snobol4blocks.1.pea $(SNOPEA_DEPS)
	$(SNOPEA) snobol4blocks.1.pea snobol4blocks.1

snobol4blocks.1.html: snobol4blocks.1.pea $(SNOPEA_DEPS)
	$(SNOPEA) snobol4blocks.1.pea snobol4blocks.1.html

snobol4blocks.1.ps: snobol4blocks.1
	$(TROFF_PS) -man snobol4blocks.1 > snobol4blocks.1.ps

####

snobol4cmd.1: snobol4cmd.1.pea $(SNOPEA_DEPS)
	$(SNOPEA) snobol4cmd.1.pea snobol4cmd.1

snobol4cmd.1.html: snobol4cmd.1.pea $(SNOPEA_DEPS)
	$(SNOPEA) snobol4cmd.1.pea snobol4cmd.1.html

snobol4cmd.1.ps: snobol4cmd.1
	$(TROFF_PS) -man snobol4cmd.1 > snobol4cmd.1.ps

####

snobol4ctrl.1: snobol4ctrl.1.pea $(SNOPEA_DEPS)
	$(SNOPEA) snobol4ctrl.1.pea snobol4ctrl.1

snobol4ctrl.1.html: snobol4ctrl.1.pea $(SNOPEA_DEPS)
	$(SNOPEA) snobol4ctrl.1.pea snobol4ctrl.1.html

snobol4ctrl.1.ps: snobol4ctrl.1
	$(TROFF_PS) -man snobol4ctrl.1 > snobol4ctrl.1.ps

####

snobol4error.1: snobol4error.1.pea $(SNOPEA_DEPS)
	$(SNOPEA) snobol4error.1.pea snobol4error.1

snobol4error.1.html: snobol4error.1.pea $(SNOPEA_DEPS)
	$(SNOPEA) snobol4error.1.pea snobol4error.1.html

snobol4error.1.ps: snobol4error.1
	$(TROFF_PS) -man snobol4error.1 > snobol4error.1.ps

####

snobol4ext.1: snobol4ext.1.pea $(SNOPEA_DEPS)
	$(SNOPEA) snobol4ext.1.pea snobol4ext.1

snobol4ext.1.html: snobol4ext.1.pea $(SNOPEA_DEPS)
	$(SNOPEA) snobol4ext.1.pea snobol4ext.1.html

snobol4ext.1.ps: snobol4ext.1
	$(TROFF_PS) -man snobol4ext.1 > snobol4ext.1.ps

####

snobol4func.1: snobol4func.1.pea $(SNOPEA_DEPS)
	$(SNOPEA) snobol4func.1.pea snobol4func.1

snobol4func.1.html: snobol4func.1.pea $(SNOPEA_DEPS)
	$(SNOPEA) snobol4func.1.pea snobol4func.1.html

snobol4func.1.ps: snobol4func.1
	$(TROFF_PS) -man snobol4func.1 > snobol4func.1.ps

####

snobol4io.1: snobol4io.1.pea $(SNOPEA_DEPS)
	$(SNOPEA) snobol4io.1.pea snobol4io.1

snobol4io.1.html: snobol4io.1.pea $(SNOPEA_DEPS)
	$(SNOPEA) snobol4io.1.pea snobol4io.1.html

snobol4io.1.ps: snobol4io.1
	$(TROFF_PS) -man snobol4io.1 > snobol4io.1.ps

####

snobol4key.1: snobol4key.1.pea $(SNOPEA_DEPS)
	$(SNOPEA) snobol4key.1.pea snobol4key.1

snobol4key.1.html: snobol4key.1.pea $(SNOPEA_DEPS)
	$(SNOPEA) snobol4key.1.pea snobol4key.1.html

snobol4key.1.ps: snobol4key.1
	$(TROFF_PS) -man snobol4key.1 > snobol4key.1.ps

####

snobol4op.1: snobol4op.1.pea $(SNOPEA_DEPS)
	$(SNOPEA) snobol4op.1.pea snobol4op.1

snobol4op.1.html: snobol4op.1.pea $(SNOPEA_DEPS)
	$(SNOPEA) snobol4op.1.pea snobol4op.1.html

snobol4op.1.ps: snobol4op.1
	$(TROFF_PS) -man snobol4op.1 > snobol4op.1.ps

####

sdb.1: sdb.1.pea $(SNOPEA_DEPS)
	$(SNOPEA) sdb.1.pea sdb.1

sdb.1.html: sdb.1.pea $(SNOPEA_DEPS)
	$(SNOPEA) sdb.1.pea sdb.1.html

sdb.1.ps: sdb.1
	$(TROFF_PS) -man sdb.1 > sdb.1.ps

####

snopea.1: ../snopea.in $(SNOPEA_DEPS)
	$(SNOPEA) ../snopea.in snopea.1

snopea.1.html: ../snopea.in $(SNOPEA_DEPS)
	$(SNOPEA) ../snopea.in snopea.1.html

snopea.1.ps: snopea.1
	$(TROFF_PS) -man snopea.1 > snopea.1.ps

################

snolib.3: snolib.3.pea $(SNOPEA_DEPS)
	$(SNOPEA) snolib.3.pea snolib.3

snolib.3.html: snolib.3.pea $(SNOPEA_DEPS)
	$(SNOPEA) snolib.3.pea snolib.3.html

snolib.3.ps: snolib.3
	$(TROFF_PS) -man snolib.3 > snolib.3.ps

####

snobol4setup.3: ../snolib/setuputil.sno $(SNOPEA_DEPS)
	$(SNOPEA) ../snolib/setuputil.sno snobol4setup.3

snobol4setup.3.html: ../snolib/setuputil.sno $(SNOPEA_DEPS)
	$(SNOPEA) ../snolib/setuputil.sno snobol4setup.3.html

################

snopea.7: snopea.7.pea $(SNOPEA_DEPS)
	$(SNOPEA) snopea.7.pea snopea.7

snopea.7.html: snopea.7.pea $(SNOPEA_DEPS)
	$(SNOPEA) snopea.7.pea snopea.7.html

snopea.7.ps: snopea.7
	$(TROFF_PS) -man snopea.7 > snopea.7.ps

####

clean:
	rm -f porting.txt *.html *.ps *.pdf \#* *~ $(GENERATED)
