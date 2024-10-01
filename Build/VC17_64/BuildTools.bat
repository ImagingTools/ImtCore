rem echo off


set COMPILER_EXT=VC17_64
set QMAKESPEC=%QTDIR%\mkspecs\win32-msvc
set path=%path%;%QTDIR%\bin

echo Generating %COMPILER_EXT% projects...

cd %~dp0\..\..\Build\QMake
%QTDIR%\bin\qmake BuildTools.pro -recursive

nmake debug
nmake release
