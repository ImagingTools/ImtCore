@echo off
REM Core test runner script for Linux containers on Windows
REM This script should NOT be modified by application repos
REM Applications should use a wrapper script that sets environment variables and calls this script

setlocal enabledelayedexpansion

REM Generate unique container name if not provided
if "%CONTAINER_NAME%"=="" set CONTAINER_NAME=myapp-tests-%RANDOM%%RANDOM%

REM Set constants and defaults
set IMAGE_NAME=imtcore-tests:linux
if "%POSTGRES_PASSWORD%"=="" set POSTGRES_PASSWORD=root
if "%POSTGRES_DB%"=="" set POSTGRES_DB=test_db

REM Auto-construct DATABASE_URL
set DATABASE_URL=postgresql://postgres:%POSTGRES_PASSWORD%@localhost:5432/%POSTGRES_DB%

echo ==========================================
echo Running Linux containers on Windows
echo ==========================================
echo.
echo [DEBUG] Script path : %~f0
echo [DEBUG] Current dir : %CD%
echo [DEBUG] Container   : %CONTAINER_NAME%
echo [DEBUG] Image       : %IMAGE_NAME%
echo [DEBUG] BASE_URL    : %BASE_URL%
echo [DEBUG] POSTGRES_DB : %POSTGRES_DB%
echo [DEBUG] POSTGRES_PASSWORD : %POSTGRES_PASSWORD%
echo [DEBUG] DATABASE_URL : %DATABASE_URL%
echo [DEBUG] TEST_USERS    : %TEST_USERS%
if "%UPDATE_SNAPSHOTS%"=="" (set UPDATE_SNAPSHOTS=false)
echo [DEBUG] UPDATE_SNAPSHOTS : %UPDATE_SNAPSHOTS%
echo.

where docker >nul 2>&1
if errorlevel 1 (
  echo ERROR: Docker not found in PATH
  exit /b 1
)

echo Checking Docker Desktop mode...
for /f "delims=" %%i in ('docker info --format "{{.OSType}}" 2^>nul') do set OS_TYPE=%%i
echo [DEBUG] Docker OSType: !OS_TYPE!
if not "!OS_TYPE!"=="linux" (
  echo ERROR: Docker Desktop is in Windows containers mode. Switch to Linux containers.
  exit /b 1
)

docker image inspect "%IMAGE_NAME%" >nul 2>&1
if errorlevel 1 (
  echo ERROR: Image not found: %IMAGE_NAME%
  echo Please build the image first using build-docker-linux-on-windows.bat
  exit /b 1
)

echo.
echo [DEBUG] Host Tests directory structure:
if exist "Tests\Startup" (
  echo [DEBUG] Host Startup *.sh:
  dir /b "Tests\Startup\*.sh" 2>nul || echo   (no .sh files)
)
if exist "Tests\GUI" (
  echo [DEBUG] Host GUI folder:
  dir /b "Tests\GUI" 2>nul || echo   (empty)
)
if exist "Tests\API" (
  echo [DEBUG] Host API folder:
  dir /b "Tests\API" 2>nul || echo   (empty)
)
echo.

REM Build volume mount arguments for application directories
set VOLUME_MOUNTS=

REM Always mount ImtCore entrypoint script from ImtCore
set IMTCORE_ENTRYPOINT=%IMTCORE_DIR%\Tests\Docker\entrypoint.sh
if exist "%IMTCORE_ENTRYPOINT%" (
  set VOLUME_MOUNTS=%VOLUME_MOUNTS% -v "%IMTCORE_ENTRYPOINT%:/app/entrypoint.sh:ro"
  echo [DEBUG] Mounting ImtCore entrypoint from: %IMTCORE_ENTRYPOINT%
)

REM Always mount ImtCore GUI utilities (utils.js, playwright.config.js, etc.) from ImtCore
set IMTCORE_GUI_DIR=%IMTCORE_DIR%\Tests\Docker\GUI
if exist "%IMTCORE_GUI_DIR%" (
  set VOLUME_MOUNTS=%VOLUME_MOUNTS% -v "%IMTCORE_GUI_DIR%:/app/tests/GUI:ro"
  echo [DEBUG] Mounting ImtCore GUI utilities from: %IMTCORE_GUI_DIR%
)

REM Mount application-specific test directories if they exist
if exist "Tests\GUI" (
  set VOLUME_MOUNTS=%VOLUME_MOUNTS% -v "%CD%\Tests\GUI:/app/tests/GUI/app:ro"
  echo [DEBUG] Mounting application GUI tests from: Tests\GUI
)

if exist "Tests\API" (
  set VOLUME_MOUNTS=%VOLUME_MOUNTS% -v "%CD%\Tests\API:/app/tests/API:ro"
  echo [DEBUG] Mounting application API tests from: Tests\API
)

if exist "Tests\Startup" (
  set VOLUME_MOUNTS=%VOLUME_MOUNTS% -v "%CD%\Tests\Startup:/app/startup:ro"
  echo [DEBUG] Mounting application startup scripts from: Tests\Startup
)

if exist "Tests\Resources" (
  set VOLUME_MOUNTS=%VOLUME_MOUNTS% -v "%CD%\Tests\Resources:/app/resources:ro"
  echo [DEBUG] Mounting application resources from: Tests\Resources
)

REM Always mount test-results as read-write for output
if not exist "%CD%\test-results" mkdir "%CD%\test-results"
set VOLUME_MOUNTS=%VOLUME_MOUNTS% -v "%CD%\test-results:/app/tests/test-results"
echo [DEBUG] Mounting test results output to: test-results

echo.
echo Starting container with volume mounts...
docker run -d ^
  --name "%CONTAINER_NAME%" ^
  --add-host=host.docker.internal:host-gateway ^
  --entrypoint sh ^
  -e BASE_URL="%BASE_URL%" ^
  -e START_POSTGRESQL=true ^
  -e POSTGRES_PASSWORD="%POSTGRES_PASSWORD%" ^
  -e POSTGRES_DB="%POSTGRES_DB%" ^
  -e DATABASE_URL="%DATABASE_URL%" ^
  -e TEST_USERS="%TEST_USERS%" ^
  -e UPDATE_SNAPSHOTS="%UPDATE_SNAPSHOTS%" ^
  -e CI=true ^
  %VOLUME_MOUNTS% ^
  "%IMAGE_NAME%" ^
  -lc "sleep infinity"

if errorlevel 1 (
  echo ERROR: Failed to start container
  exit /b 1
)

docker ps -a --filter "name=%CONTAINER_NAME%"

echo.
echo [DEBUG] Container mounted directories:
docker exec "%CONTAINER_NAME%" sh -lc "echo '--- /app/startup ---'; ls -la /app/startup 2>/dev/null || echo '(empty)'; echo '--- /app/tests/GUI ---'; ls -la /app/tests/GUI 2>/dev/null || echo '(empty)'; echo '--- /app/tests/API ---'; ls -la /app/tests/API 2>/dev/null || echo '(empty)'"

echo.
echo Running tests (entrypoint)...
docker exec "%CONTAINER_NAME%" sh -lc "export PAUSE_BEFORE_TESTS=false; /app/entrypoint.sh"
set EXIT_CODE=%ERRORLEVEL%
echo [DEBUG] Test run exit code: %EXIT_CODE%

echo.
echo Test results are available in: test-results\

echo.
echo ==========================================
echo Container logs:
echo ==========================================
docker logs "%CONTAINER_NAME%"

echo.
echo Cleaning up container...
docker rm -f "%CONTAINER_NAME%" >nul 2>&1

echo ==========================================
if "%EXIT_CODE%"=="0" (
  echo Tests passed successfully!
) else (
  echo Tests failed with exit code: %EXIT_CODE%
)
echo ==========================================

exit /b %EXIT_CODE%
