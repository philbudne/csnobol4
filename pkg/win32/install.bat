rem $Id$
rem install batch file for CSNOBOL4 when building from source

setlocal

if "%1" != "" set SNOLIB=%1
rem Default set in config\win32\config.h:
if not DEFINED SNOLIB set SNOLIB=C:\snobol4

mkdir %SNOLIB%
mkdir %SNOLIB%\local

rem read file created by Un*x configure shell script;
set /p VERSION=<version
set SNOVER=%SNOLIB%\%VERSION%

mkdir %SNOVER%
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

mkdir %BINDIR%
copy /y cpuid.exe %BINDIR%
copy /y snobol4.exe %BINDIR%
copy /y pkg\win32\sdb.bat %BINDIR%
copy /y pkg\win32\timing.bat %BINDIR%

mkdir %LIBDIR%
copy /y host.sno %LIBDIR%
copy /y snolib\*.sno %LIBDIR%
copy /y modules\setuputil.sno %LIBDIR%
copy /y config\win32\config.sno %LIBDIR%

mkdir %DLLDIR%
config\win32\modules.bat install

mkdir %DOCDIR%
copy /y doc\*.html %DOCDIR%
copy /y doc\load.doc %DOCDIR%\load.txt
copy /y modules\*\*.html %DOCDIR%

mkdir %INCDIR%
copy /y equ.h %INCDIR%
copy /y version.h %INCDIR%
copy /y include\h.h %INCDIR%
copy /y include\load.h %INCDIR%
copy /y include\macros.h %INCDIR%
copy /y include\snotypes.h %INCDIR%
copy /y config\win32\config.h %INCDIR%

mkdir %TIMDIR%
copy /y timing.sno %TIMDIR%
copy /y test\procs %TIMDIR%
copy /y test\globals %TIMDIR%
copy /y test\v311.sil %TIMDIR%
copy /y test\bench.sno %TIMDIR%

mkdir %SNOVER%\local
mkdir %SNOVER%\local\shared
