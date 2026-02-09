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

echo Starting idle container (entrypoint will NOT run yet^)...
docker run -d ^
  --name %CONTAINER_NAME% ^
  --entrypoint cmd ^
  -e BASE_URL=%BASE_URL% ^
  -e START_POSTGRESQL=false ^
  -e POSTGRES_PASSWORD=%POSTGRES_PASSWORD% ^
  -e POSTGRES_DB=%POSTGRES_DB% ^
  -e DATABASE_URL=%DATABASE_URL% ^
  -e TEST_USERS=%TEST_USERS% ^
  -e CI=true ^
  %IMAGE_NAME% ^
  /c timeout /t 3600

if errorlevel 1 (
    echo ERROR: Failed to start container
    exit /b 1
)

docker ps -a --filter "name=%CONTAINER_NAME%"

REM Ensure target dirs exist
echo.
echo Creating directories in container...
docker exec %CONTAINER_NAME% cmd /c "mkdir C:\app\tests\GUI C:\app\tests\API C:\app\startup C:\app\resources 2>nul"

echo.
echo Copying GUI tests...
if exist "Tests\GUI" (
    docker cp "Tests\GUI\." "%CONTAINER_NAME%:C:\app\tests\GUI\"
) else (
    echo WARNING: Tests\GUI directory not found - skipping
)

echo Copying API tests...
if exist "Tests\API" (
    docker cp "Tests\API\." "%CONTAINER_NAME%:C:\app\tests\API\"
) else (
    echo WARNING: Tests\API directory not found - skipping
)

echo Copying resources...
if exist "Tests\Resources" (
    docker cp "Tests\Resources\." "%CONTAINER_NAME%:C:\app\resources\"
) else (
    echo WARNING: Tests\Resources directory not found - skipping
)

echo Copying startup scripts...
if exist "Tests\Startup" (
    docker cp "Tests\Startup\." "%CONTAINER_NAME%:C:\app\startup\"
) else (
    echo WARNING: Tests\Startup directory not found - skipping
)

echo.
echo [DEBUG] Container files after copy:
docker exec %CONTAINER_NAME% cmd /c "dir C:\app\startup & dir C:\app\tests\GUI & dir C:\app\tests\API"

echo.
echo Running tests (entrypoint^)...
docker exec %CONTAINER_NAME% cmd /c "set PAUSE_BEFORE_TESTS=false && C:\app\entrypoint.bat"
set EXIT_CODE=%ERRORLEVEL%

echo [DEBUG] Test run exit code: %EXIT_CODE%

set RESULTS_DIR=%~dp0test-results

echo.
echo Copying test results...
if exist "%RESULTS_DIR%" (
    rmdir /s /q "%RESULTS_DIR%" 2>nul
)
docker cp "%CONTAINER_NAME%:C:\app\tests\test-results" "%RESULTS_DIR%" 2>nul
if errorlevel 1 (
    echo No test results to copy
)

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
