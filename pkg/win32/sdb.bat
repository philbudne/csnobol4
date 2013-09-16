@echo off
rem Phil Budne 8/14/2013

setlocal
set SDB_LISTFILE=%TMP%\sdb-%RANDOM%-%TIME:~6,5%.lst
set BINDIR=%~dp0

rem start in new window -- allows ^C trapping
start %BINDIR%\snobol4 -b -l %SDB_LISTFILE% -L %BINDIR%\..\sdb.sno %*
