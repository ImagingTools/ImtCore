@echo off
setlocal

REM ===============================================================
REM Prints the full executable path of a running process (first
REM match), or nothing if it is not running. Used to remember where
REM a plain-exe product (ProLife/Lisa/Puma) was launched from before
REM stopping it, so it can be relaunched from the same place if the
REM supervising service does not bring it back up automatically.
REM
REM Usage:
REM   GetProcessPath.bat <processName>   (without .exe, matches Get-Process -Name)
REM
REM Prints one line (the path) to stdout on success, nothing if the
REM process is not currently running.
REM ===============================================================

set "PROC_NAME=%~1"
if "%PROC_NAME%"=="" (
    echo ERROR: GetProcessPath.bat requires a process name ^(without .exe^) as the first argument 1>&2
    exit /b 1
)

powershell -NoProfile -NonInteractive -Command "(Get-Process -Name '%PROC_NAME%' -ErrorAction SilentlyContinue | Select-Object -First 1 -ExpandProperty Path)"

endlocal
