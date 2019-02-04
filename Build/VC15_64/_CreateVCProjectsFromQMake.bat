rem echo off


set COMPILER_EXT=VC15_64
set QMAKESPEC=%QTDIR%\mkspecs\win32-msvc
set path=%path%;%QTDIR%\bin

echo Generating %COMPILER_EXT% projects...

cd %~dp0\..\..\Build\QMake
%QTDIR%\bin\qmake -recursive -tp vc


cd %~dp0\..\..
call %ACFCONFIGDIR%\QMake\CopyVCProjToSubdir.js %COMPILER_EXT%

cd %~dp0\
