rem $Id$
rem install batch file for CSNOBOL4 when building from source

setlocal

if NOT "%1" == "" set SNOLIB=%1
rem Default set in config\win32\config.h:
if not DEFINED SNOLIB set SNOLIB=C:\snobol4

if not exist %SNOLIB% mkdir %SNOLIB%
if not exist %SNOLIB%\local mkdir %SNOLIB%\local

rem read file created by Un*x configure shell script;
set /p VERSION=<version
set SNOVER=%SNOLIB%\%VERSION%

if not exist %SNOVER% mkdir %SNOVER%
copy /y README %SNOVER%
copy /y CHANGES %SNOVER%
copy /y COPYRIGHT %SNOVER%
copy /y snobol4.lib %SNOVER%
copy /y pkg\win32\README.win32 %SNOVER%

set BINDIR=%SNOVER%\bin
set LIBDIR=%SNOVER%\lib
set DLLDIR=%SNOVER%\lib\shared
set DOCDIR=%SNOVER%\doc
set INCDIR=%SNOVER%\include
set TIMDIR=%SNOVER%\timing

if not exist %BINDIR% mkdir %BINDIR%
copy /y cpuid.exe %BINDIR%
copy /y snobol4.exe %BINDIR%
copy /y pkg\win32\sdb.bat %BINDIR%
copy /y pkg\win32\timing.bat %BINDIR%
copy /y pkg\win32\snopea.bat %BINDIR%

if not exist %LIBDIR% mkdir %LIBDIR%
copy /y host.sno %LIBDIR%
copy /y snopea.in %LIBDIR%\snopeacmd.sno
copy /y snolib\*.sno %LIBDIR%
copy /y config\win32\config.sno %LIBDIR%

if not exist %DOCDIR% mkdir %DOCDIR%
copy /y doc\*.html %DOCDIR%
rem copy /y doc\load.doc %DOCDIR%\load.txt

if not exist %INCDIR% mkdir %INCDIR%
copy /y equ.h %INCDIR%
copy /y version.h %INCDIR%
copy /y include\h.h %INCDIR%
copy /y include\load.h %INCDIR%
copy /y include\macros.h %INCDIR%
copy /y include\snotypes.h %INCDIR%
copy /y config\win32\config.h %INCDIR%

if not exist %TIMDIR% mkdir %TIMDIR%
copy /y timing.sno %TIMDIR%
copy /y test\procs %TIMDIR%
copy /y test\globals %TIMDIR%
copy /y test\v311.sil %TIMDIR%
copy /y test\bench.sno %TIMDIR%

if not exist %SNOVER%\local mkdir %SNOVER%\local
if not exist %SNOVER%\local\shared mkdir %SNOVER%\local\shared

if not exist %DLLDIR% mkdir %DLLDIR%
call config\win32\modules.bat install
