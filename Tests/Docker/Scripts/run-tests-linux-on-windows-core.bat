@echo off
REM Core test runner script for Linux containers on Windows
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
set IMAGE_NAME=imtcore-tests:linux
if not defined POSTGRES_PASSWORD set POSTGRES_PASSWORD=root
if not defined POSTGRES_DB set POSTGRES_DB=test_db

REM Auto-construct DATABASE_URL
if defined POSTGRES_DB (
    set DATABASE_URL=postgresql://postgres:%POSTGRES_PASSWORD%@localhost:5432/%POSTGRES_DB%
)

echo ==========================================
echo Running Linux containers on Windows
echo ==========================================
echo.
echo [DEBUG] Script path: %~f0
echo [DEBUG] Current dir: %CD%
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

REM Check Docker Desktop mode
echo Checking Docker Desktop mode...
for /f "delims=" %%i in ('docker info --format "{{.OSType}}" 2^>nul') do set OS_TYPE=%%i
echo [DEBUG] Docker OSType: !OS_TYPE!
if not "!OS_TYPE!"=="linux" (
    echo ERROR: Docker Desktop is in Windows containers mode. Switch to Linux containers.
    exit /b 1
)

REM Check if image exists
docker image inspect %IMAGE_NAME% >nul 2>&1
if errorlevel 1 (
    echo ERROR: Docker image '%IMAGE_NAME%' not found
    echo Please build the image first using build-docker-linux-on-windows.bat
    exit /b 1
)

echo [DEBUG] Host Tests directory structure:
if exist "Tests\Startup" (
    echo [DEBUG] Host Startup *.sh:
    dir /b "Tests\Startup\*.sh" 2>nul
    if errorlevel 1 echo   (no .sh files^)
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

REM Get ImtCore Docker directory (parent of Scripts folder where this script is located)
set IMTCORE_DOCKER_DIR=%~dp0..
for %%i in ("%IMTCORE_DOCKER_DIR%") do set IMTCORE_DOCKER_DIR=%%~fi

REM Check that entrypoint.sh exists
if not exist "%IMTCORE_DOCKER_DIR%\entrypoint.sh" (
    echo ERROR: ImtCore entrypoint not found at: %IMTCORE_DOCKER_DIR%\entrypoint.sh
    exit /b 1
)

REM Clean and create test-results directory
echo [DEBUG] Cleaning previous test results...
if exist "%CD%\Tests\test-results" (
    del /f /s /q "%CD%\Tests\test-results\*" >nul 2>&1
    rmdir /s /q "%CD%\Tests\test-results" >nul 2>&1
)
mkdir "%CD%\Tests\test-results"
echo [DEBUG] Test results directory cleaned: Tests\test-results

REM Only mount test-results for output (read-write)
set VOLUME_MOUNTS=-v "%CD%\Tests\test-results:/app/tests/test-results"
echo [DEBUG] Mounting test results output to: Tests\test-results

echo.
echo Starting container...
docker run -d ^
    --name %CONTAINER_NAME% ^
    --add-host=host.docker.internal:host-gateway ^
    --entrypoint sh ^
    -e BASE_URL=%BASE_URL% ^
    -e START_POSTGRESQL=true ^
    -e POSTGRES_PASSWORD=%POSTGRES_PASSWORD% ^
    -e POSTGRES_DB=%POSTGRES_DB% ^
    -e DATABASE_URL=%DATABASE_URL% ^
    -e TEST_USERS=%TEST_USERS% ^
    -e UPDATE_SNAPSHOTS=%UPDATE_SNAPSHOTS% ^
    -e CI=true ^
    %VOLUME_MOUNTS% ^
    %IMAGE_NAME% ^
    -c "sleep infinity"

if errorlevel 1 (
    echo ERROR: Failed to start container
    exit /b 1
)

docker ps -a --filter "name=%CONTAINER_NAME%"

echo.
echo ==========================================
echo Copying files to container...
echo ==========================================

REM Clean and copy ImtCore files
echo [DEBUG] Cleaning /app/imtcore...
docker exec %CONTAINER_NAME% sh -c "rm -rf /app/imtcore && mkdir -p /app/imtcore"
echo [DEBUG] Copying ImtCore Docker directory...
docker cp "%IMTCORE_DOCKER_DIR%\." %CONTAINER_NAME%:/app/imtcore/

REM Clean and copy ImtCore GUI utilities
if exist "%IMTCORE_DOCKER_DIR%\GUI" (
    echo [DEBUG] Cleaning /app/tests/GUI...
    docker exec %CONTAINER_NAME% sh -c "rm -rf /app/tests/GUI && mkdir -p /app/tests/GUI"
    echo [DEBUG] Copying ImtCore GUI utilities...
    docker cp "%IMTCORE_DOCKER_DIR%\GUI\." %CONTAINER_NAME%:/app/tests/GUI/
)

REM Clean and copy application GUI tests
if exist "Tests\GUI" (
    echo [DEBUG] Cleaning /app/tests/GUI/app...
    docker exec %CONTAINER_NAME% sh -c "rm -rf /app/tests/GUI/app && mkdir -p /app/tests/GUI/app"
    echo [DEBUG] Copying application GUI tests...
    docker cp "%CD%\Tests\GUI\." %CONTAINER_NAME%:/app/tests/GUI/app/
)

REM Clean and copy application API tests
if exist "Tests\API" (
    echo [DEBUG] Cleaning /app/tests/API...
    docker exec %CONTAINER_NAME% sh -c "rm -rf /app/tests/API && mkdir -p /app/tests/API"
    echo [DEBUG] Copying application API tests...
    docker cp "%CD%\Tests\API\." %CONTAINER_NAME%:/app/tests/API/
)

REM Clean and copy startup scripts
if exist "Tests\Startup" (
    echo [DEBUG] Cleaning /app/startup...
    docker exec %CONTAINER_NAME% sh -c "rm -rf /app/startup && mkdir -p /app/startup"
    echo [DEBUG] Copying startup scripts...
    docker cp "%CD%\Tests\Startup\." %CONTAINER_NAME%:/app/startup/
)

REM Clean and copy resources
if exist "Tests\Resources" (
    echo [DEBUG] Cleaning /app/resources...
    docker exec %CONTAINER_NAME% sh -c "rm -rf /app/resources && mkdir -p /app/resources"
    echo [DEBUG] Copying resources...
    docker cp "%CD%\Tests\Resources\." %CONTAINER_NAME%:/app/resources/
)

echo.
echo [DEBUG] Container directories after copy:
docker exec %CONTAINER_NAME% sh -c "echo '--- /app/imtcore ---' && ls -la /app/imtcore 2>/dev/null || echo '(empty)' && echo '--- /app/startup ---' && ls -la /app/startup 2>/dev/null || echo '(empty)' && echo '--- /app/tests/GUI ---' && ls -la /app/tests/GUI 2>/dev/null || echo '(empty)' && echo '--- /app/tests/API ---' && ls -la /app/tests/API 2>/dev/null || echo '(empty)'"

echo.
echo Converting CRLF to LF for shell scripts...
docker exec %CONTAINER_NAME% sh -c "find /app/imtcore -name '*.sh' -exec sed -i 's/\r$//' {} \; 2>/dev/null; find /app/startup -name '*.sh' -exec sed -i 's/\r$//' {} \; 2>/dev/null; chmod +x /app/imtcore/*.sh 2>/dev/null; chmod +x /app/startup/*.sh 2>/dev/null; echo 'Done'"

echo.
echo ==========================================
echo Running tests (entrypoint)...
echo ==========================================
docker exec %CONTAINER_NAME% sh -c "export PAUSE_BEFORE_TESTS=false && /app/imtcore/entrypoint.sh"
set EXIT_CODE=%ERRORLEVEL%

echo [DEBUG] Test run exit code: %EXIT_CODE%

echo.
echo Test results are available in: Tests\test-results\

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