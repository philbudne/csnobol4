@echo off
rem $Id$
rem SNOBOL4 tiny "POD" formatter
rem Phil Budne 12/27/2014

setlocal
set "BINDIR=%~dp0"
set "LIBDIR=%BINDIR%\..\lib"

"%BINDIR%\snobol4" "%LIBDIR%\snopeacmd.sno" %*
