@echo off

color 3a
:start
cls

gmake -f Makefile

:end
set a=
set /p a=��ȷ�ϼ�ɾ�������ļ�:
if "%a%"=="" goto del
goto exit

:del
echo ɾ�������ļ�...
del obj_err\*.o
del obj_err\*.deps
del obj_err\*.err
echo ɾ���ɹ�