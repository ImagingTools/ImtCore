@echo off
REM Build ImtCore Docker image for Windows containers
REM This script builds the Windows Docker image

setlocal

set IMAGE_NAME=imtcore-tests:windows

echo ==========================================
echo Building ImtCore Docker Test Image
echo (Windows^)
echo ==========================================
echo.

REM Configuration
set DOCKERFILE=Tests\Docker\Dockerfile.windows

REM Check if Docker is installed
docker --version >nul 2>&1
if errorlevel 1 (
    echo Error: Docker is not installed or not in PATH
    exit /b 1
)

REM Check if Dockerfile exists
if not exist "%DOCKERFILE%" (
    echo Error: Dockerfile not found at %DOCKERFILE%
    echo Make sure you're running this script from the ImtCore repository root
    exit /b 1
)

echo Building Windows image...
echo Image: %IMAGE_NAME%
echo Using Dockerfile: %DOCKERFILE%
echo.

REM Build the Docker image
docker build -f %DOCKERFILE% -t %IMAGE_NAME% .

if errorlevel 0 (
    echo.
    echo ==========================================
    echo [SUCCESS] Build successful!
    echo ==========================================
    echo Image: %IMAGE_NAME%
    echo.
    echo To verify the image:
    echo   docker images ^| findstr imtcore-tests
    echo.
    echo To use this image from an application repository:
    echo   1. Copy Tests\Docker\Scripts\run-tests-windows-template.bat to your application repo
    echo   2. Run: run-tests.bat
    echo.
) else (
    echo.
    echo ==========================================
    echo [FAILED] Build failed!
    echo ==========================================
    exit /b 1
)
