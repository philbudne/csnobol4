@echo off
rem Phil Budne 8/14/2013

rem UGH. variable visible system wide!!!
:LOOP
set SDB_LISTFILE=%TMP%\sdb-%RANDOM%-%TIME:~6,5%.lst
if exist "%SDB_LISTFILE%" GOTO :LOOP

setlocal
set SNODIR=%HOMEDRIVE%\snobol4
%SNODIR%\snobol4 -b -l %SDB_LISTFILE% -L %SNODIR%\sdb.sno %*
