setlocal
set CMD=%1
if "%CMD%" == "" set CMD=build

set TOP=%CD%
set SNOPATH=%TOP%;%TOP%\snolib;%TOP%\config\win32

cd "%TOP%\modules\com"
"%TOP%\snobol4" -N setup.sno -v %CMD%

cd "%TOP%\modules\dirs"
"%TOP%\snobol4" -N setup.sno -v %CMD%

cd "%TOP%\modules\logic"
"%TOP%\snobol4" -N setup.sno -v %CMD%

cd "%TOP%\modules\ndbm"
"%TOP%\snobol4" -N setup.sno -v %CMD%

cd "%TOP%\modules\random"
"%TOP%\snobol4" -N setup.sno -v %CMD%

cd "%TOP%\modules\sprintf"
"%TOP%\snobol4" -N setup.sno -v %CMD%

cd "%TOP%\modules\sqlite3"
if EXIST sqlite3.c "%TOP%\snobol4" -N setup.sno -v %CMD%

cd "%TOP%\modules\stat"
"%TOP%\snobol4" -N setup.sno -v %CMD%

cd "%TOP%\modules\time"
"%TOP%\snobol4" -N setup.sno -v %CMD%

cd "%TOP%"
