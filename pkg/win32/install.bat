rem $Id$
rem install batch file for CSNOBOL4 when building from source

setlocal

rem NOTE!! Compiled in defaults are in config\win32\config.h
set DEST=%1
if NOT DEFINED DEST set DEST=C:\

set SNOBASE=%DEST%\snobol4
mkdir %SNOBASE%
mkdir %SNOBASE%\local

rem read file created by Un*x configure shell script;
set /p VERSION=<version
set SNODIR=%SNOBASE%\%VERSION%
mkdir %SNODIR%
copy README %SNODIR%
copy CHANGES %SNODIR%
copy COPYRIGHT %SNODIR%
copy pkg\win32\README.win32 %SNODIR%

set BINDIR=%SNODIR%\bin
set LIBDIR=%SNODIR%\lib
set DLLDIR=%SNODIR%\lib\shared
set DOCDIR=%SNODIR%\doc
set INCDIR=%SNODIR%\include
set TIMDIR=%SNODIR%\timing

mkdir %BINDIR%
copy cpuid.exe %BINDIR%
copy snobol4.exe %BINDIR%
copy pkg\win32\sdb.bat %BINDIR%
copy pkg\win32\timing.bat %BINDIR%

mkdir %LIBDIR%
copy host.sno %LIBDIR%
copy snobol4.lib %LIBDIR%
copy snolib\*.sno %LIBDIR%
copy config\win32\config.sno %LIBDIR%

mkdir %DLLDIR%
copy modules\*\*.dll %DLLDIR%

mkdir %DOCDIR%
copy doc\*.html %DOCDIR%
copy doc\load.doc %DOCDIR%
copy modules\*\*.html %DOCDIR%

mkdir %INCDIR%
copy config\win32\config.h %INCDIR%
copy equ.h %INCDIR%
copy version.h %INCDIR%
copy include\h.h %INCDIR%
copy include\snotypes.h %INCDIR%
copy include\macros.h %INCDIR%
copy include\load.h %INCDIR%

mkdir %TIMDIR%
copy timing.sno %TIMDIR%
copy test\v311.sil %TIMDIR%
copy test\procs %TIMDIR%
copy test\globals %TIMDIR%
copy test\bench.sno %TIMDIR%

mkdir %SNODIR%\local
mkdir %SNODIR%\local\shared
