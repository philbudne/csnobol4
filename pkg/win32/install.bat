rem $Id$

mkdir %2

copy equ.h %2
copy include\dt.h %2
copy include\equ.h %2
copy include\h.h %2
copy include\load.h %2
copy include\macros.h %2
copy include\snotypes.h %2
copy config\%1\config.h %2

copy snolib\*.sno %2
copy host.sno %2

copy doc\load.doc %2

copy snobol4.exe %2
