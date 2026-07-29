@echo off
setlocal EnableDelayedExpansion

REM ===============================================================
REM Polls `tasklist` until a process image is running or has exited.
REM Used for products that run as plain executables supervised by
REM another process/service rather than being registered as their
REM own Windows service (e.g. ProLife/Lisa/Puma under AgentinoAgent).
REM
REM Usage:
REM   WaitForProcess.bat <imageName.exe> <RUNNING|STOPPED> [timeoutSeconds=60]
REM
REM Exit code 0 if the state was reached, 1 if the timeout elapsed first.
REM ===============================================================

set "IMAGE_NAME=%~1"
set "TARGET_STATE=%~2"
set "TIMEOUT_SEC=%~3"
if "%TIMEOUT_SEC%"=="" set "TIMEOUT_SEC=60"

if "%IMAGE_NAME%"=="" (
    echo ERROR: WaitForProcess.bat requires a process image name as the first argument
    exit /b 1
)
if "%TARGET_STATE%"=="" (
    echo ERROR: WaitForProcess.bat requires a target state ^(RUNNING or STOPPED^) as the second argument
    exit /b 1
)

set /a ELAPSED=0

:poll_loop
set "IS_RUNNING=0"
tasklist /FI "IMAGENAME eq %IMAGE_NAME%" /NH 2>nul | findstr /i /c:"%IMAGE_NAME%" >nul
if !errorlevel! == 0 set "IS_RUNNING=1"

if "%TARGET_STATE%"=="RUNNING" if "!IS_RUNNING!"=="1" (
    echo %IMAGE_NAME% is running after %ELAPSED%s
    exit /b 0
)
if "%TARGET_STATE%"=="STOPPED" if "!IS_RUNNING!"=="0" (
    echo %IMAGE_NAME% is stopped after %ELAPSED%s
    exit /b 0
)

if !ELAPSED! GEQ %TIMEOUT_SEC% (
    echo WARNING: %IMAGE_NAME% did not reach state %TARGET_STATE% within %TIMEOUT_SEC%s
    exit /b 1
)

REM `timeout` requires an interactive console and fails immediately
REM ("Input redirection is not supported") on a non-interactive
REM TeamCity build agent, so use the classic ping-based sleep instead.
ping -n 3 127.0.0.1 >nul
set /a ELAPSED+=2
goto :poll_loop
