# $Id$

CPP=/lib/cpp

## include $(MAKEFLAGS)?? 
it:	Makefile2
	$(MAKE) -f Makefile2 


Makefile2: config.h Makefile2.cpp
	echo '# DO NOT EDIT. machine generated from Makefile.cpp' > Makefile2
	$(CPP) Makefile2.cpp | grep -v '^#' | cat -s >> Makefile2
	echo '# DO NOT DELETE THIS LINE. make depend uses it.' >> Makefile2
	make -f Makefile2 depend

config.h:
	@echo 'copy (or link) a suitable file from config/ to config.h'
	@exit 1

clean:	Makefile2
	$(MAKE) -f Makefile2 clean

tar:	Makefile2
	$(MAKE) -f Makefile2 tar

