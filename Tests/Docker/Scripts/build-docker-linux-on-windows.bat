@echo off
REM Build ImtCore Docker image for Linux containers running on Windows
REM This script builds the Linux Docker image on Windows using Docker Desktop

echo ==========================================
echo Building ImtCore Docker Test Image
echo (Linux containers on Windows)
echo ==========================================
echo.

REM Configuration
if "%IMAGE_NAME%"=="" set IMAGE_NAME=imtcore-tests:linux
set DOCKERFILE=Tests/Docker/Dockerfile.linux

REM Check if Docker is installed
docker --version >nul 2>&1
if errorlevel 1 (
    echo Error: Docker is not installed or not in PATH
    exit /b 1
)

REM Check Docker mode
echo Checking Docker Desktop mode...
for /f "delims=" %%i in ('docker info --format "{{.OSType}}" 2^>nul') do set OS_TYPE=%%i

if not "%OS_TYPE%"=="linux" (
    echo.
    echo WARNING: Docker Desktop is in Windows containers mode!
    echo.
    echo To build Linux containers on Windows:
    echo   1. Right-click Docker Desktop icon in system tray
    echo   2. Select 'Switch to Linux containers...'
    echo   3. Wait for Docker to restart
    echo   4. Run this script again
    echo.
    exit /b 1
)

echo [OK] Docker Desktop is in Linux containers mode
echo.

REM Check if Dockerfile exists
if not exist "%DOCKERFILE%" (
    echo Error: Dockerfile not found at %DOCKERFILE%
    echo Make sure you're running this script from the ImtCore repository root
    exit /b 1
)

echo Building Linux image on Windows...
echo Image: %IMAGE_NAME%
echo Using Dockerfile: %DOCKERFILE%
echo.

REM Build the Docker image
docker build -f "%DOCKERFILE%" -t "%IMAGE_NAME%" .

if errorlevel 1 (
    echo.
    echo ==========================================
    echo [FAILED] Build failed!
    echo ==========================================
    exit /b 1
) else (
    echo.
    echo ==========================================
    echo [SUCCESS] Build successful!
    echo ==========================================
    echo Image: %IMAGE_NAME%
    echo.
    echo This Linux container can now run on Windows via Docker Desktop (WSL 2)
    echo.
    echo To verify the image:
    echo   docker images ^| findstr imtcore-tests
    echo.
    echo To use this image from an application repository:
    echo   1. Copy Tests\Docker\run-tests-linux-on-windows.sh to your application repo
    echo   2. Run in Git Bash or WSL: ./run-tests-linux-on-windows.sh
    echo.
    echo Note: Linux containers on Windows use WSL 2 backend
    echo       This provides near-native Linux performance
    echo.
)
