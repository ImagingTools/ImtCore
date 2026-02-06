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

REM Check if IMTCOREDIR environment variable is set
if "%IMTCOREDIR%"=="" (
  echo ERROR: IMTCOREDIR environment variable is not set.
  echo Please set it to the path of your ImtCore directory:
  echo   set IMTCOREDIR=C:\path\to\ImtCore
  exit /b 1
)

REM Validate ImtCore path
if not exist "%IMTCOREDIR%\Tests\Docker\Scripts\run-tests-linux-on-windows-core.bat" (
  echo ERROR: Cannot find core script at: %IMTCOREDIR%\Tests\Docker\Scripts\run-tests-linux-on-windows-core.bat
  echo Please ensure IMTCOREDIR points to a valid ImtCore directory.
  exit /b 1
)

call "%IMTCOREDIR%\Tests\Docker\Scripts\run-tests-linux-on-windows-core.bat"
exit /b %ERRORLEVEL%
