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

REM Get script directory (should be Tests\)
set SCRIPT_DIR=%~dp0

REM Set container details
set IMAGE_NAME=imtcore-tests:linux
set CONTAINER_NAME=myapp-tests-%RANDOM%

REM Auto-construct DATABASE_URL
set DATABASE_URL=postgresql://postgres:%POSTGRES_PASSWORD%@localhost:5432/%POSTGRES_DB%

echo ==========================================
echo Running Linux containers on Windows
echo ==========================================
echo Container: %CONTAINER_NAME%
echo Image: %IMAGE_NAME%
echo BASE_URL: %BASE_URL%
echo.

REM Check if Docker is installed
docker --version >nul 2>&1
if %ERRORLEVEL% neq 0 (
    echo ERROR: Docker is not installed or not in PATH
    exit /b 1
)

REM Check Docker mode
echo Checking Docker Desktop mode...
for /f "tokens=*" %%i in ('docker info --format "{{.OSType}}" 2^>nul') do set OS_TYPE=%%i

if not "%OS_TYPE%"=="linux" (
    echo ERROR: Docker Desktop is in Windows containers mode!
    echo Please switch to Linux containers mode
    exit /b 1
)

echo ✓ Docker Desktop is in Linux containers mode
echo.

REM Check if image exists
docker image inspect %IMAGE_NAME% >nul 2>&1
if %ERRORLEVEL% neq 0 (
    echo ERROR: Docker image '%IMAGE_NAME%' not found
    echo Please build the image first using ImtCore build scripts
    exit /b 1
)

REM Run container with volume mounts
echo Starting container with test files...
docker run --rm ^
    --name %CONTAINER_NAME% ^
    --add-host=host.docker.internal:host-gateway ^
    -e BASE_URL=%BASE_URL% ^
    -e POSTGRES_DB=%POSTGRES_DB% ^
    -e POSTGRES_PASSWORD=%POSTGRES_PASSWORD% ^
    -e DATABASE_URL=%DATABASE_URL% ^
    -e TEST_USERNAME=%TEST_USERNAME% ^
    -e TEST_PASSWORD=%TEST_PASSWORD% ^
    -v "%SCRIPT_DIR%GUI:/mnt/app/GUI:ro" ^
    -v "%SCRIPT_DIR%API:/mnt/app/API:ro" ^
    -v "%SCRIPT_DIR%..\Resources:/mnt/app/Resources:ro" ^
    -v "%SCRIPT_DIR%..\Startup:/mnt/app/Startup:ro" ^
    -v "%SCRIPT_DIR%package.json:/mnt/app/package.json:ro" ^
    %IMAGE_NAME% ^
    /app/copy-and-run.sh

set EXIT_CODE=%ERRORLEVEL%

echo.
echo ==========================================
echo Tests completed with exit code: %EXIT_CODE%
echo ==========================================

exit /b %EXIT_CODE%
