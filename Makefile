# $Id$

M4=m4

## include $(MAKEFLAGS)?? 
it:	Makefile2
	$(MAKE) -f Makefile2 


M2TMP=Makefile2.tmp
Makefile2: config.m4 Makefile2.m4
	echo '# DO NOT EDIT. machine generated from Makefile2.m4' > $(M2TMP)
	$(M4) Makefile2.m4 >> $(M2TMP)
	echo '# DO NOT DELETE THIS LINE. make depend uses it.' >> $(M2TMP)
	make -f $(M2TMP) depend MAKEFILE2=$(M2TMP)
	mv $(M2TMP) Makefile2

config.h:
	@echo 'copy (or link) a suitable file from config/ to config.h'
	@exit 1

clean:	Makefile2
	$(MAKE) -f Makefile2 clean

tar:	Makefile2
	$(MAKE) -f Makefile2 tar

uu:	Makefile2
	$(MAKE) -f Makefile2 uu
