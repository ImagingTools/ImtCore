@echo off
REM Template wrapper script for running tests in application repos
REM Copy this file to your application repo's Tests directory and configure it for your needs

REM ==========================================
REM CONFIGURATION - MODIFY THESE FOR YOUR APP
REM ==========================================

REM Your application URL (use host.docker.internal to access Windows host)
set BASE_URL=http://host.docker.internal:7776

REM PostgreSQL settings (set START_POSTGRESQL=false if you don't need PostgreSQL)
set START_POSTGRESQL=true
set POSTGRES_DB=myapp_test
set DATABASE_URL=postgresql://postgres:postgres@localhost:5432/myapp_test

REM Test user credentials
set TEST_USERNAME=test@example.com
set TEST_PASSWORD=testpassword

REM Docker image name (usually don't need to change this)
set IMAGE_NAME=imtcore-tests:linux

REM ==========================================
REM CORE LOGIC - DO NOT MODIFY BELOW THIS LINE
REM ==========================================

REM Find ImtCore path (adjust this path to point to your ImtCore location)
set IMTCORE_PATH=%~dp0..\..\..\..\ImtCore
if not exist "%IMTCORE_PATH%\Tests\Docker\Scripts\run-tests-linux-on-windows-core.bat" (
  echo ERROR: Cannot find ImtCore at %IMTCORE_PATH%
  echo Please update IMTCORE_PATH in this script to point to your ImtCore directory
  exit /b 1
)

REM Call the core script from ImtCore
call "%IMTCORE_PATH%\Tests\Docker\Scripts\run-tests-linux-on-windows-core.bat"
exit /b %ERRORLEVEL%
