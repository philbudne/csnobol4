# $Id$

M4=m4

it:	Makefile2
	$(MAKE) -f Makefile2 $(TARGET)

.PRECIOUS: Makefile2

xsnobol4: Makefile2
	$(MAKE) -f Makefile2 xsnobol4

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

config.m4:
	./autoconf > config.m4.tmp
	mv -f config.m4.tmp config.m4

# XXX set to .depend?
M2DEP=Makefile2

# leave binary
clean:	$(M2DEP)
	$(MAKE) -f Makefile2 clean

# make ready for compilation on another platform
realclean: $(M2DEP)
	$(MAKE) -f Makefile2 realclean

# removes generated files!!
spotless: $(M2DEP)
	$(MAKE) -f Makefile2 spotless

tar:	$(M2DEP)
	$(MAKE) -f Makefile2 tar

uu:	$(M2DEP)
	$(MAKE) -f Makefile2 uu

install: $(M2DEP)
	$(MAKE) -f Makefile2 install
