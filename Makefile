# $Id$

# TARGETS:
# snobol4	make binary & regression test
# xsnobol4	make binary, no regression test
# tar		make distribution
# uu		make uuencoded distribution
# clean		leave binary, config files; removes objects
# realclean	make ready for compilation on another platform
# distclean	clean as when unpacked
# spotless	removes snobol4 generated files

# m4 macro processor; used to generate Makefile2
# (largely to avoid depending on make '+=' operator)
M4=m4

# compression program, suffix
COMP=gzip
Z=gz

################
# snobol commands

# bootstrapped using Catspaw SPARC SPITBOL
#SNO=spitbol -i512k -b
#SMALL_SNO=spitbol -b
SNO=snobol4 -b
SMALL_SNO=snobol4 -b

################
# machine generated files;

GENERATED=data.c data_init.h proc.h syn.h data.h \
	equ.h res.h syn_init.h isnobol4.c snobol4.c host.sno

################
# SIL source file
SIL=	v311.sil

################
# AIX4 makes all targets, so added this;
# must be named "all" for FreeBSD "ports"?

all:	snobol4

# force GNU make to run top Makefile serially
.NOTPARALLEL: foo

# files to avoid removing when interrupted
#	add $(GENERATED)?
.PRECIOUS: snobol4 xsnobol4 Makefile2 snobol4.c isnobol4.c snobol4 data_init.h 

snobol4 xsnobol4 install: $(GENERATED) Makefile2 ALWAYS .depend
	$(MAKE) -f Makefile2 $@

ALWAYS:

# for hand generation of sources
generated: $(GENERATED)

################
# run configuration script

config.m4 config.h:
	./configure

################
# make second level makefile

M2TMP=Makefile2.tmp
Makefile2 .depend: config.m4 Makefile2.m4
	echo '# DO NOT EDIT. machine generated from Makefile2.m4' > $(M2TMP)
	echo '# add local changes to local-config'		>> $(M2TMP)
	$(M4) Makefile2.m4 >> $(M2TMP)
	echo '# DO NOT DELETE THIS LINE. make depend uses it.' >> $(M2TMP)
	make -f $(M2TMP) depend MAKEFILE2=$(M2TMP)
	mv -f $(M2TMP) Makefile2
	rm -f .depend
	touch .depend

################
# code

# regular version
snobol4.c proc.h2: procs genc.sno globals $(SIL) 
	rm -f snobol4.c2 proc.h2
	$(SNO) genc.sno $(SIL) > snobol4.c2
	mv -f snobol4.c2 snobol4.c

# generate inline version (functions reordered)

# Uses tsort (topological sort), to order routines for best inlining.
# private copy of FreeBSD version of tsort (topological sort) supplied
# since GNU version used on Linux and Cygwin can't handle cycles

# isnobol4.c can't depend on bsdtsort, since that would force
# isnobol4.c to be rebuilt (which requires a snobol4 binary) when
# starting with a distribution kit.  Using a SNOBOL4 tsort.sno program
# would solve the problem, but I've yet to write one that works as
# well as the C version.

isnobol4.c: procs genc.sno globals $(SIL)
	rm -rf isnobol4.c2 proc.h2 prolog subr
	test -f bsdtsort || $(CC) -o bsdtsort bsdtsort.c
	mkdir subr
	$(SNO) -- genc.sno --inline $(SIL) > prolog
	cd subr && ../bsdtsort < ../callgraph > order && \
		cat ../prolog `cat order` > ../isnobol4.c2
	mv -f isnobol4.c2 isnobol4.c
	rm -rf prolog subr

proc.h:	proc.h2
	@cmp proc.h proc.h2 || cp proc.h2 proc.h

################
# syntax tables

# only change syn.h if it has changed from last run!

syn.c syn.h2 syn_init.h2: syntax.tbl gensyn.sno
	rm -f syn.c2 syn.h2
	$(SNO) gensyn.sno syntax.tbl
	mv -f syn.c2 syn.c

syn.h:	syn.h2
	@cmp syn.h syn.h2 || cp syn.h2 syn.h

syn_init.h: syn_init.h2
	@cmp syn_init.h syn_init.h2 || cp syn_init.h2 syn_init.h

################
# resident data

data.h2 data.c2 equ.h2 data_init.h2 res.h2: $(SIL) gendata.sno
	rm -f data.h2 data.c2 equ.h2 data_init.h2 res.h2
	$(SNO) gendata.sno $(SIL)

data.h:	data.h2
	@cmp data.h data.h2 || cp data.h2 data.h

data.c:	data.c2
	@cmp data.c data.c2 || cp data.c2 data.c

equ.h:	equ.h2
	@cmp equ.h equ.h2 || cp equ.h2 equ.h

res.h:	res.h2
	@cmp res.h res.h2 || cp res.h2 res.h

data_init.h: data_init.h2
	@cmp data_init.h data_init.h2 || cp data_init.h2 data_init.h

#################
# generated snolib files

host.sno: host.awk lib/snolib/host.h
	awk -f host.awk lib/snolib/host.h > host.sno

#################
# dependency generation is slow and ugly (and wrong?)
# doesn't change much!!

#procs:	gendep.sno
#	$(SNO) gendep.sno < $(SIL) > procs2
#	mv -f procs2 procs

##################################################################
# housekeeping

# generated files to include in kit (hard link to target dir)
G1=data.c2 data.h2 data_init.h2 proc.h2 equ.h2 syn.h2 syn_init.h2 \
	res.h2 snobol4.c isnobol4.c 

# generated files to include in kit (copy, so newer than .x2 versions)
G2=data.c data.h data_init.h proc.h equ.h res.h syn.c syn.h syn_init.h \
	host.sno

# disposables
DISP=*.o *.a callgraph prolog bsplitu pv vers build.c bsdtsort

# remove objects, turds; leave generated sources, final binary, Makefile2
clean:
	rm -f $(DISP)
	rm -rf subr
	find . -type f -a \( \
		-name '*~' -o -name '#*' -o -name '*.tmp' -o -name '.#*' \) \
		-print | xargs -t rm -f

# make ready for compilation on another platform (leave binaries)
realclean: clean
	rm -f config.m4 config.h Makefile2 .depend

# clean as when it was unpacked; remove binaries too
distclean: realclean
	rm -f snobol4 xsnobol4 timing.out

# remove objects, generated files
# leave only final binary??
spotless: distclean
	rm -f $(G1) $(G2) snobol4.c isnobol4.c snobol xsnobol4

# generated files copied separately to ensure newer than source files!
TAR=	README CHANGES History INSTALL TODO TODO.soon doc Makefile \
	Makefile2.m4 configure config.guess $(SIL) syntax.tbl \
	procs globals genc.sno gensyn.sno gendata.sno main.c \
	data_init.c version.c parms.h mlink.h mdata.h pml.h $(G1) lib \
	include config test snolib/*.sno sunmodel timing timing.sno \
	cc-M bsplitu.c host.awk pkg

# "print version" -- for dir/tar names
pv:	version.c
	$(CC) -I./include -DMAIN -o pv version.c

# tempting to use VERS:sh = ./pv
# but need to make pv first!!
VERS=`./pv`

DIR=snobol-$(VERS)
KIT=snobol-$(VERS).tar.$(Z)

tar vers: clean snobol4 pv
	cd doc; make
	cd test; ./clean.sh
	rm -rf snobol-[0-9]*
	rm -f *~ */*~ */*/*~ *.tmp
	mkdir $(DIR)
	find $(TAR) -name RCS -prune -o -print | cpio -pldm $(DIR)
	cp $(G2) $(DIR)
	tar cf - $(DIR) | $(COMP) > $(KIT)
	rm -rf $(DIR)
	./pv > vers

# make a mailable kit by splitting up binary, and uuencoding pieces
# (no editing necessary for reassembly)
# requires "./pv" for $(VERS) in $(KIT)
uu:	bsplitu tar pv
	rm -rf uu; mkdir uu
	cd uu; ln ../pv .; ../bsplitu $(KIT) < ../$(KIT); rm pv

bsplitu: bsplitu.c
	$(CC) -o bsplitu bsplitu.c
