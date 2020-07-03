cd doc
set SNOPEA=..\snobol4.exe -N -I.. -I..\snolib ..\snopea.in

for %%F in (*.pea) do %SNOPEA% %%F %%~nF.html

:: self-documenting source not located in doc directory:

:: snopea command
%SNOPEA% ..\snopea.in snopea.1.html

:: libraries:
%SNOPEA% ..\host.sno snobol4host.3.html
%SNOPEA% ..\snolib\setuputil.sno snobol4setup.3.html
%SNOPEA% ..\snolib\ezio.sno snobol4ezio.3.html

cd ..
