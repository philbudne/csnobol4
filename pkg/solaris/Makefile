BASE=../..
pkg:	clean
	mkdir CSNOBOL4 REL
	cp $(BASE)/COPYRIGHT CSNOBOL4/copyright
	cp pkginfo prototype CSNOBOL4
	mkdir -p CSNOBOL4/lib/snobol4 CSNOBOL4/bin CSNOBOL4/man/man1
	cp $(BASE)/snobol4 CSNOBOL4/bin
	cp $(BASE)/snolib/*.sno CSNOBOL4/lib/snobol4
	cp $(BASE)/doc/load.txt $(BASE)/host.sno CSNOBOL4/lib/snobol4
	cp $(BASE)/include/*.h CSNOBOL4/lib/snobol4
	cp $(BASE)/config.h $(BASE)/equ.h CSNOBOL4/lib/snobol4
	cp $(BASE)/doc/snobol4.1 CSNOBOL4/man/man1
	cd CSNOBOL4; pkgmk -o -r . -d ../REL
	pkgtrans -s REL CSNOBOL4.stream CSNOBOL4

clean:
	rm -rf CSNOBOL4 REL
