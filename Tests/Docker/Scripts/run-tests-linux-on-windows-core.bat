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

echo Starting idle container (entrypoint will NOT run yet)...
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
  -e CI=true ^
  "%IMAGE_NAME%" ^
  -lc "sleep infinity"

if errorlevel 1 (
  echo ERROR: Failed to start container
  exit /b 1
)

docker ps -a --filter "name=%CONTAINER_NAME%"

REM Ensure target dirs exist
echo.
echo Creating directories in container...
docker exec "%CONTAINER_NAME%" sh -lc "mkdir -p /app/tests/GUI /app/tests/API /app/startup /app/resources"

echo.
echo Copying GUI tests...
if exist "Tests\GUI" (
  docker cp "Tests\GUI\." "%CONTAINER_NAME%:/app/tests/GUI/"
) else (
  echo WARNING: Tests\GUI directory not found - skipping
)

echo Copying API tests...
if exist "Tests\API" (
  docker cp "Tests\API\." "%CONTAINER_NAME%:/app/tests/API/"
) else (
  echo WARNING: Tests\API directory not found - skipping
)

echo Copying resources...
if exist "Tests\Resources" (
  docker cp "Tests\Resources\." "%CONTAINER_NAME%:/app/resources/"
) else (
  echo WARNING: Tests\Resources directory not found - skipping
)

echo Copying startup scripts...
if exist "Tests\Startup" (
  docker cp "Tests\Startup\." "%CONTAINER_NAME%:/app/startup/"
  docker exec "%CONTAINER_NAME%" sh -lc "chmod +x /app/startup/*.sh 2>/dev/null || true"
) else (
  echo WARNING: Tests\Startup directory not found - skipping
)

echo.
echo [DEBUG] Container files after copy:
docker exec "%CONTAINER_NAME%" sh -lc "echo '--- /app/startup ---'; ls -la /app/startup 2>/dev/null || echo '(empty)'; echo '--- /app/tests/GUI ---'; ls -la /app/tests/GUI 2>/dev/null || echo '(empty)'; echo '--- /app/tests/API ---'; ls -la /app/tests/API 2>/dev/null || echo '(empty)'"

echo.
echo Running tests (entrypoint)...
docker exec "%CONTAINER_NAME%" sh -lc "export PAUSE_BEFORE_TESTS=false; /app/entrypoint.sh"
set EXIT_CODE=%ERRORLEVEL%
echo [DEBUG] Test run exit code: %EXIT_CODE%

set "RESULTS_DIR=%~dp0test-results"

echo.
echo Copying test results...
if exist "%RESULTS_DIR%" rmdir /s /q "%RESULTS_DIR%" >nul 2>&1
docker cp "%CONTAINER_NAME%:/app/tests/test-results" "%RESULTS_DIR%" 2>nul || echo No test results to copy

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
