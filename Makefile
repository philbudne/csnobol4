# $Id$

CPP=/lib/cpp

## include $(MAKEFLAGS)?? 
it:	Makefile2
	$(MAKE) -f Makefile2 


Makefile2: config.h Makefile2.cpp
	$(CPP) Makefile2.cpp | grep -v '^#' | cat -s > Makefile2
#	make -f Makefile2 depend

config.h:
	@echo 'copy (or link) a suitable file from config/ to config.h'
	@exit 1
