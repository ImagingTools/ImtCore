@echo off
REM Simple configuration file for running Docker tests (Linux containers on Windows)
REM Copy this file to your application's Tests directory and configure the variables below

REM ==========================================
REM CONFIGURATION
REM ==========================================

REM Application URL (use host.docker.internal to access Windows host)
set BASE_URL=http://host.docker.internal:7776

REM PostgreSQL database name
set POSTGRES_DB=myapp_test

REM PostgreSQL password (default: root)
set POSTGRES_PASSWORD=root

REM Test credentials
set TEST_USERNAME=test@example.com
set TEST_PASSWORD=testpassword

REM ==========================================
REM DO NOT MODIFY BELOW THIS LINE
REM ==========================================

REM Auto-discover ImtCore (looks in parent directories)
set IMTCORE_PATH=%~dp0..\..
if not exist "%IMTCORE_PATH%\Tests\Docker\Scripts\run-tests-linux-on-windows-core.bat" (
  set IMTCORE_PATH=%~dp0..\..\..
)
if not exist "%IMTCORE_PATH%\Tests\Docker\Scripts\run-tests-linux-on-windows-core.bat" (
  set IMTCORE_PATH=%~dp0..\..\..\..
)
if not exist "%IMTCORE_PATH%\Tests\Docker\Scripts\run-tests-linux-on-windows-core.bat" (
  echo ERROR: Cannot find ImtCore. Please ensure ImtCore is in a parent directory.
  exit /b 1
)

call "%IMTCORE_PATH%\Tests\Docker\Scripts\run-tests-linux-on-windows-core.bat"
exit /b %ERRORLEVEL%
