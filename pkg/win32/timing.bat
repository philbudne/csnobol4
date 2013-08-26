@echo off
:: Phil Budne 8/14/2013

setlocal
set SNODIR=%SYSTEMDRIVE%\snobol4

echo Date:
echo %date% %time%
echo:

echo ver:
ver
echo:

echo PROCESSOR_ARCHITECTURE:
echo %PROCESSOR_ARCHITECTURE%
echo:

echo PROCESSOR_IDENTIFIER:
echo %PROCESSOR_IDENTIFIER%
echo:

echo hostname:
hostname
echo:

echo cpuid:
%SNODIR%\cpuid
echo:

cd %SNODIR%\timing
echo Ids:
find "Id:" bench.sno v311.sil timing.sno
echo:

echo running benchmark:
..\snobol4 -s bench.sno v311.sil 2>stderr
..\snobol4 -b timing.sno < stderr

echo:
echo END
