@echo =========== %1
@s4 -b -r %2 %3 %4 %1.sno > %1.lst
@cmp %1.ref %1.lst
