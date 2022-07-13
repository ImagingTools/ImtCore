echo off

set ACFDIR=%~dp0..\..\..\Acf
set ACFCONFIGDIR=%~dp0..\..\..\Acf\Config
set ACFSLNDIR=%~dp0..\..\..\AcfSln
set IACFDIR=%~dp0..\..\..\IAcf
set IMTCOREDIR=%~dp0..\

set LICENSE_TYPE=OpenSource
set LICENSE_DIR=Install\LGPL
set LICENSE_INSTALL_PATH=..\LGPL\License.txt
set ACF_CONFIG_FILE=$(IMTCOREDIR)\Config\ImtCore.awc

cd %~dp0\..

set COMPILER_EXT=Qt5_VC15_x64
call %ACFDIR%\Install\InternalParts\CreateTempDirs.bat

call %ACFDIR%\Install\InternalParts\CreateScripts.bat

call %ACFDIR%\Install\InternalParts\Create_TechnicalDoc.bat

call %ACFDIR%\Install\InternalParts\CopyTempFiles.bat
