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
copy /y README %SNODIR%
copy /y CHANGES %SNODIR%
copy /y COPYRIGHT %SNODIR%
copy /y pkg\win32\README.win32 %SNODIR%

set BINDIR=%SNODIR%\bin
set LIBDIR=%SNODIR%\lib
set DLLDIR=%SNODIR%\lib\shared
set DOCDIR=%SNODIR%\doc
set INCDIR=%SNODIR%\include
set TIMDIR=%SNODIR%\timing

mkdir %BINDIR%
copy /y cpuid.exe %BINDIR%
copy /y snobol4.exe %BINDIR%
copy /y pkg\win32\sdb.bat %BINDIR%
copy /y pkg\win32\timing.bat %BINDIR%

mkdir %LIBDIR%
copy /y host.sno %LIBDIR%
copy /y snobol4.lib %LIBDIR%
copy /y snolib\*.sno %LIBDIR%
copy /y modules\setuputil.sno %LIBDIR%
copy /y config\win32\config.sno %LIBDIR%

rem XXX loop running setup.sno "install" !!!!
mkdir %DLLDIR%
mkdir %DOCDIR%
copy /y doc\*.html %DOCDIR%
copy /y doc\load.doc %DOCDIR%

config\win32\modules.bat install

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

mkdir %SNODIR%\local
mkdir %SNODIR%\local\shared
