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

# AIX4 makes all targets, so added this;
default: snobol4

snobol4 xsnobol4 tar uu install: Makefile2 ALWAYS
	$(MAKE) -f Makefile2 $@

# disposables
DISP=*.o *.a callgraph prolog bsplitu pv vers

# remove objects; leave generated sources, final binary, Makefile2
clean:
	rm -f $(DISP) *~ */*~ */*/*~ *.tmp \#*
	rm -rf subr

# make ready for compilation on another platform (leave binaries)
realclean: clean
	rm -f config.m4 Makefile2

# clean as when it was unpacked; remove binaries too
distclean: realclean
	rm -f snobol4 xsnobol4

.PRECIOUS: snobol4 xsnobol4

ALWAYS:

################
# run configuration script

config.m4:
	./configure

################
# make second level makefile

.PRECIOUS: Makefile2

M4=m4

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
