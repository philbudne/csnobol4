cd doc
for %%F in (*.pea) do ..\snobol4.exe -N -I.. -I..\snolib ..\snopea.in %%F %%~nF.html
..\snobol4.exe -N -I.. -I..\snolib ..\snopea.in ..\snopea.in snopea.1.html
cd ..
