@echo off
rem Phil Budne 8/14/2013

setlocal
:LOOP
set SDB_LISTFILE=%TMP%\sdb-%RANDOM%-%TIME:~6,5%.lst
if exist "%SDB_LISTFILE%" GOTO :LOOP

set SNODIR=%SYSTEMDRIVE%\snobol4
%SNODIR%\snobol4 -b -l %SDB_LISTFILE% -L %SNODIR%\sdb.sno %*
