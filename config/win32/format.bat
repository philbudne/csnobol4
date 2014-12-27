cd doc
for %%F in (*.pea) do ..\snobol4 -N -I.. -I..\snolib ..\snopea.in %%F %%~nF.html
cd ..
