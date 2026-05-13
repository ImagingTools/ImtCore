@echo off
setlocal enabledelayedexpansion

cd /d "%~dp0\..\.."

set "FILE=Partitura\ImtCoreVoce.arp\VersionInfo.acc.xtrsvn"
set "BACKUPDIR=%~1"

git fetch --prune --unshallow 2>nul

for /f "usebackq delims=" %%i in (`git rev-list --count origin/main 2^>nul`) do set REV=%%i

if not defined REV (
    for /f "usebackq delims=" %%i in (`git rev-list --count HEAD 2^>nul`) do set REV=%%i
)

if not defined REV (
    echo Failed to compute revision count.
    exit /b 1
)

set /a REV_OFFSET=REV+10000

git diff-index --quiet HEAD --
if %errorlevel%==0 (
    set DIRTY=0
) else (
    set DIRTY=1
)

echo Git revision: %REV% (version: %REV_OFFSET%), dirty: %DIRTY%
echo Processing file: %FILE%

set "OUT=%FILE:.xtrsvn=%"
set "TMP=%OUT%.tmp"

if not "%BACKUPDIR%"=="" (
    set "BACKUPFILE=%BACKUPDIR%\%OUT%"
    if not exist "%OUT%" (
        if exist "!BACKUPFILE!" (
            copy /y "!BACKUPFILE!" "%OUT%" >nul
            echo Restored %OUT% from backup !BACKUPFILE!
        )
    )
)

(for /f "usebackq delims=" %%L in ("%FILE%") do (
    set "line=%%L"
    set "line=!line:$WCREV$=%REV_OFFSET%!"
    set "line=!line:$WCMODS?1:0$=%DIRTY%!"
    echo(!line!
)) > "%TMP%"

if exist "%OUT%" (
    fc /b "%TMP%" "%OUT%" >nul
    if errorlevel 1 (
        move /y "%TMP%" "%OUT%" >nul
        echo Wrote %OUT% with WCREV=%REV_OFFSET% and WCMODS=%DIRTY%
    ) else (
        del "%TMP%" >nul 2>&1
        echo No changes in %OUT%, file not rewritten
    )
) else (
    move /y "%TMP%" "%OUT%" >nul
    echo Wrote %OUT% with WCREV=%REV_OFFSET% and WCMODS=%DIRTY%
)

if not "%BACKUPDIR%"=="" (
    for %%D in ("!BACKUPFILE!") do if not exist "%%~dpD" mkdir "%%~dpD"
    copy /y "%OUT%" "!BACKUPFILE!" >nul
    echo Backed up %OUT% to !BACKUPFILE!
)

endlocal
exit /b 0
