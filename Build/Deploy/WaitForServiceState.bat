@echo off
setlocal EnableDelayedExpansion

REM ===============================================================
REM Polls `sc query` until a Windows service reaches the requested
REM state, instead of a blind fixed-length timeout. Used by the
REM TeamCity release pipelines' install steps when stopping/starting
REM a service before/after running a silent installer.
REM
REM Usage:
REM   WaitForServiceState.bat <serviceName> <RUNNING|STOPPED> [timeoutSeconds=60]
REM
REM Exit code 0 if the state was reached, 1 if the timeout elapsed
REM first or the service does not exist. The caller decides whether
REM that is fatal.
REM ===============================================================

set "SERVICE_NAME=%~1"
set "TARGET_STATE=%~2"
set "TIMEOUT_SEC=%~3"
if "%TIMEOUT_SEC%"=="" set "TIMEOUT_SEC=60"

if "%SERVICE_NAME%"=="" (
    echo ERROR: WaitForServiceState.bat requires a service name as the first argument
    exit /b 1
)
if "%TARGET_STATE%"=="" (
    echo ERROR: WaitForServiceState.bat requires a target state ^(RUNNING or STOPPED^) as the second argument
    exit /b 1
)

set "STATE_FILE=%TEMP%\_waitforservicestate_%RANDOM%.txt"
set /a ELAPSED=0

:poll_loop
sc query "%SERVICE_NAME%" > "%STATE_FILE%" 2>&1

findstr /i /c:"%TARGET_STATE%" "%STATE_FILE%" >nul
if !errorlevel! == 0 (
    echo %SERVICE_NAME% reached state %TARGET_STATE% after %ELAPSED%s
    del "%STATE_FILE%" >nul 2>&1
    exit /b 0
)

findstr /i /c:"FAILED" "%STATE_FILE%" >nul
if !errorlevel! == 0 (
    echo WARNING: service %SERVICE_NAME% could not be queried ^(not installed?^), skipping wait
    del "%STATE_FILE%" >nul 2>&1
    exit /b 1
)

if !ELAPSED! GEQ %TIMEOUT_SEC% (
    echo WARNING: %SERVICE_NAME% did not reach state %TARGET_STATE% within %TIMEOUT_SEC%s
    del "%STATE_FILE%" >nul 2>&1
    exit /b 1
)

del "%STATE_FILE%" >nul 2>&1
REM `timeout` requires an interactive console and fails immediately
REM ("Input redirection is not supported") on a non-interactive
REM TeamCity build agent, so use the classic ping-based sleep instead.
ping -n 3 127.0.0.1 >nul
set /a ELAPSED+=2
goto :poll_loop
