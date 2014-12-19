set SNOPATH=..;..\..;..\..\snolib;..\..\config\win32

cd modules\logic
..\..\snobol4 -N setup.sno build

cd ..\ndbm
..\..\snobol4 -N setup.sno build

cd ..\com
..\..\snobol4 -N setup.sno build

cd ..\sprintf
..\..\snobol4 -N setup.sno build

cd ..\stat
..\..\snobol4 -N setup.sno build

cd ..\dirs
..\..\snobol4 -N setup.sno build

cd ..\time
..\..\snobol4 -N setup.sno build

cd ..\..
