echo off
cls
set /A i = 45

:Func

set /A i = %i% - 1
Proj.exe
if i equ 0 goto end
goto Func

:end
pause
exit