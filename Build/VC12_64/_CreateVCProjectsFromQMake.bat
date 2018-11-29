@echo off

set COMPILER_EXT=VC12_64
set QMAKESPEC=%QTDIR%\mkspecs\win32-msvc2013
set path=%path%;%QTDIR%\bin

cd %~dp0\..\..

echo Generating %COMPILER_EXT% projects...
cd Build\QMake
%QTDIR%\bin\qmake -recursive -tp vc
cd %~dp0\..\..

call %ACFDIR%\Config\QMake\CopyVCProjToSubdir.js %COMPILER_EXT% -replace%QTDIR%=$(QTDIR) -replace%ACFDIR%\=$(ACFDIR)\ -replace%ACFSLNDIR%\=$(ACFSLNDIR)\ -replace%ACF_TOOLS_BIN%\=$(ACF_TOOLS_BIN)\ -replace%CD%\=$(RelativePath)\


cd %~dp0\
