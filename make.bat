@echo off

color 3a
:start
cls

gmake -f Makefile

:end
set a=
set /p a=按确认键删除过程文件:
if "%a%"=="" goto del
goto exit

:del
echo 删除过程文件...
del obj_err\*.o
del obj_err\*.deps
del obj_err\*.err
echo 删除成功