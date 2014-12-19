set TOP=%CD%
set SNOPATH=%TOP%\modules;%TOP%;%TOP%\snolib;%TOP%\config\win32


cd "%TOP%\modules\com"
"%TOP%\snobol4" -N setup.sno build

cd "%TOP%\modules\dirs"
"%TOP%\snobol4" -N setup.sno build

cd "%TOP%\modules\logic"
"%TOP%\snobol4" -N setup.sno build

cd "%TOP%\modules\ndbm"
"%TOP%\snobol4" -N setup.sno build

cd "%TOP%\modules\sprintf"
"%TOP%\snobol4" -N setup.sno build

cd "%TOP%\modules\stat"
"%TOP%\snobol4" -N setup.sno build

cd "%TOP%\modules\time"
"%TOP%\snobol4" -N setup.sno build

cd "%TOP%"
