@echo off
REM Core test runner script for Windows containers on Windows
REM This script should NOT be modified by application repos
REM Applications should use a wrapper script that sets environment variables and calls this script

setlocal enabledelayedexpansion

REM Generate unique container name if not provided
if not defined CONTAINER_NAME (
    for /f "tokens=2-4 delims=/ " %%a in ('date /t') do set mydate=%%c%%a%%b
    for /f "tokens=1-2 delims=/:" %%a in ('time /t') do set mytime=%%a%%b
    set mytime=!mytime: =0!
    set CONTAINER_NAME=imtcore-tests-!mydate!!mytime!
)

REM Set constants and defaults
set IMAGE_NAME=imtcore-tests:windows
if not defined POSTGRES_PASSWORD set POSTGRES_PASSWORD=root
if not defined POSTGRES_DB set POSTGRES_DB=test_db

REM Auto-construct DATABASE_URL (Note: PostgreSQL typically not available in Windows containers)
if defined POSTGRES_DB (
    set DATABASE_URL=postgresql://postgres:%POSTGRES_PASSWORD%@localhost:5432/%POSTGRES_DB%
)

echo ==========================================
echo Running Windows containers on Windows
echo ==========================================
echo.
echo [DEBUG] Container: %CONTAINER_NAME%
echo [DEBUG] Image: %IMAGE_NAME%
echo [DEBUG] BASE_URL: %BASE_URL%
echo [DEBUG] POSTGRES_DB: %POSTGRES_DB%
echo [DEBUG] POSTGRES_PASSWORD: %POSTGRES_PASSWORD%
echo [DEBUG] DATABASE_URL: %DATABASE_URL%
echo [DEBUG] TEST_USERS: %TEST_USERS%
if not defined UPDATE_SNAPSHOTS set UPDATE_SNAPSHOTS=false
echo [DEBUG] UPDATE_SNAPSHOTS: %UPDATE_SNAPSHOTS%
echo.

REM Check if Docker is available
docker --version >nul 2>&1
if errorlevel 1 (
    echo ERROR: Docker is not installed or not in PATH
    exit /b 1
)

REM Check if image exists
docker image inspect %IMAGE_NAME% >nul 2>&1
if errorlevel 1 (
    echo ERROR: Docker image '%IMAGE_NAME%' not found
    echo Please build the image first using build-docker-windows.bat
    exit /b 1
)

echo [DEBUG] Host Tests directory structure:
if exist "Tests\Startup" (
    echo [DEBUG] Host Startup *.bat:
    dir /b "Tests\Startup\*.bat" 2>nul
    if errorlevel 1 echo   (no .bat files^)
)
if exist "Tests\GUI" (
    echo [DEBUG] Host GUI folder:
    dir /b "Tests\GUI" 2>nul
)
if exist "Tests\API" (
    echo [DEBUG] Host API folder:
    dir /b "Tests\API" 2>nul
)
echo.

REM Build volume mount arguments for application directories
set VOLUME_MOUNTS=

REM Get ImtCore Docker directory (parent of Scripts folder where this script is located)
set IMTCORE_DOCKER_DIR=%~dp0..
for %%i in ("%IMTCORE_DOCKER_DIR%") do set IMTCORE_DOCKER_DIR=%%~fi

REM Check that entrypoint.bat exists
if not exist "%IMTCORE_DOCKER_DIR%\entrypoint.bat" (
    echo ERROR: ImtCore entrypoint not found at: %IMTCORE_DOCKER_DIR%\entrypoint.bat
    exit /b 1
)

REM Mount ImtCore Docker directory (contains entrypoint.bat)
set VOLUME_MOUNTS=%VOLUME_MOUNTS% -v "%IMTCORE_DOCKER_DIR%:C:\app\imtcore"
echo [DEBUG] Mounting ImtCore Docker directory from: %IMTCORE_DOCKER_DIR%

REM Mount ImtCore GUI utilities if they exist
if exist "%IMTCORE_DOCKER_DIR%\GUI" (
    set VOLUME_MOUNTS=%VOLUME_MOUNTS% -v "%IMTCORE_DOCKER_DIR%\GUI:C:\app\tests\GUI"
    echo [DEBUG] Mounting ImtCore GUI utilities from: %IMTCORE_DOCKER_DIR%\GUI
)

REM Mount application-specific test directories if they exist
if exist "Tests\GUI" (
    set VOLUME_MOUNTS=%VOLUME_MOUNTS% -v "%CD%\Tests\GUI:C:\app\tests\GUI\app"
    echo [DEBUG] Mounting application GUI tests from: Tests\GUI
)

if exist "Tests\API" (
    set VOLUME_MOUNTS=%VOLUME_MOUNTS% -v "%CD%\Tests\API:C:\app\tests\API"
    echo [DEBUG] Mounting application API tests from: Tests\API
)

if exist "Tests\Startup" (
    set VOLUME_MOUNTS=%VOLUME_MOUNTS% -v "%CD%\Tests\Startup:C:\app\startup"
    echo [DEBUG] Mounting application startup scripts from: Tests\Startup
)

if exist "Tests\Resources" (
    set VOLUME_MOUNTS=%VOLUME_MOUNTS% -v "%CD%\Tests\Resources:C:\app\resources"
    echo [DEBUG] Mounting application resources from: Tests\Resources
)

REM Always mount test-results as read-write for output
if not exist "%CD%\test-results" mkdir "%CD%\test-results"
set VOLUME_MOUNTS=%VOLUME_MOUNTS% -v "%CD%\test-results:C:\app\tests\test-results"
echo [DEBUG] Mounting test results output to: test-results

echo.
echo Starting container with volume mounts...
docker run -d ^
  --name %CONTAINER_NAME% ^
  --entrypoint cmd ^
  -e BASE_URL=%BASE_URL% ^
  -e START_POSTGRESQL=true ^
  -e POSTGRES_PASSWORD=%POSTGRES_PASSWORD% ^
  -e POSTGRES_DB=%POSTGRES_DB% ^
  -e DATABASE_URL=%DATABASE_URL% ^
  -e TEST_USERS=%TEST_USERS% ^
  -e UPDATE_SNAPSHOTS=%UPDATE_SNAPSHOTS% ^
  -e CI=false ^
  %VOLUME_MOUNTS% ^
  %IMAGE_NAME% ^
  /c "C:\Windows\System32\WindowsPowerShell\v1.0\powershell.exe -NoProfile -Command Start-Sleep -Seconds 3600"

if errorlevel 1 (
    echo ERROR: Failed to start container
    exit /b 1
)

docker ps -a --filter "name=%CONTAINER_NAME%"

echo.
echo [DEBUG] Container mounted directories:
docker exec %CONTAINER_NAME% cmd /c "dir C:\app\startup & dir C:\app\tests\GUI & dir C:\app\tests\API"

echo.
echo Running tests (entrypoint^)...
docker exec %CONTAINER_NAME% cmd /c "set PAUSE_BEFORE_TESTS=false && C:\app\imtcore\entrypoint.bat"
set EXIT_CODE=%ERRORLEVEL%

echo [DEBUG] Test run exit code: %EXIT_CODE%

echo.
echo Test results are available in: test-results\

echo.
echo ==========================================
echo Container logs:
echo ==========================================
docker logs %CONTAINER_NAME%

echo.
echo Cleaning up container...
docker rm -f %CONTAINER_NAME% >nul 2>&1

echo ==========================================
if %EXIT_CODE% equ 0 (
    echo Tests passed successfully!
) else (
    echo Tests failed with exit code: %EXIT_CODE%
)
echo ==========================================

exit /b %EXIT_CODE%
