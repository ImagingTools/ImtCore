echo off

set LICENSE_TYPE=LGPL
set LICENSE_DIR=Install/LGPL
set LICENSE_INSTALL_PATH=../LGPL/License.txt
set ACF_CONFIG_FILE=%~dp0/../Config/AcfSlnConfig.awc

call %~dp0\..\Build\VC15_64\_CreateVCProjectsFromQMake.bat

cd %~dp0\..

set COMPILER_EXT=VC15_64
call %ACFDIR%\Install\InternalParts\CreateTempDirs.bat

call Install\InternalParts\CopyDlls.bat

echo Compiling...
vcbuild /r %~dp0\..\Build\%COMPILER_EXT%\AcfSlnAll.sln $All
echo Compiling done

call %ACFDIR%\Install\InternalParts\CreateScripts.bat

call %ACFDIR%\Install\InternalParts\Create_TechnicalDoc.bat

call %ACFDIR%\Install\InternalParts\CopyTempFiles.bat

call %ACFDIR%\Install\InternalParts\Compile_Setup.bat

echo Installation done

call %ACFDIR%\Install\InternalParts\RemoveTempDirs.bat
