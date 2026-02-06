@echo off
REM Run tests using Linux containers on Windows

setlocal enabledelayedexpansion

set CONTAINER_NAME=myapp-tests-%RANDOM%%RANDOM%
if "%IMAGE_NAME%"=="" set IMAGE_NAME=imtcore-tests:linux
REM IMPORTANT: localhost inside container == container itself; prefer host.docker.internal when app runs on Windows host
if "%BASE_URL%"=="" set BASE_URL=
if "%START_POSTGRESQL%"=="" set START_POSTGRESQL=true
if "%POSTGRES_DB%"=="" set POSTGRES_DB=
if "%DATABASE_URL%"=="" set DATABASE_URL=
if "%TEST_USERNAME%"=="" set TEST_USERNAME=
if "%TEST_PASSWORD%"=="" set TEST_PASSWORD=

echo ==========================================
echo Running Linux containers on Windows
echo ==========================================
echo.
echo [DEBUG] Script path : %~f0
echo [DEBUG] Current dir : %CD%
echo [DEBUG] Container   : %CONTAINER_NAME%
echo [DEBUG] Image       : %IMAGE_NAME%
echo [DEBUG] BASE_URL    : %BASE_URL%
echo.

where docker >nul 2>&1
if errorlevel 1 (
  echo Docker not found in PATH
  exit /b 1
)

echo Checking Docker Desktop mode...
for /f "delims=" %%i in ('docker info --format "{{.OSType}}" 2^>nul') do set OS_TYPE=%%i
echo [DEBUG] Docker OSType: !OS_TYPE!
if not "!OS_TYPE!"=="linux" (
  echo Docker Desktop is in Windows containers mode. Switch to Linux containers.
  exit /b 1
)

docker image inspect "%IMAGE_NAME%" >nul 2>&1
if errorlevel 1 (
  echo Image not found: %IMAGE_NAME%
  exit /b 1
)

echo.
echo [DEBUG] Host Startup *.sh:
dir /b "Tests\Startup\*.sh" 2>nul
echo [DEBUG] Host GUI folder:
dir /b "Tests\GUI" 2>nul
echo [DEBUG] Host API folder:
dir /b "Tests\API" 2>nul
echo.

echo Starting idle container (entrypoint will NOT run yet)...
docker run -d ^
  --name "%CONTAINER_NAME%" ^
  --add-host=host.docker.internal:host-gateway ^
  --entrypoint sh ^
  -e BASE_URL="%BASE_URL%" ^
  -e START_POSTGRESQL="%START_POSTGRESQL%" ^
  -e POSTGRES_DB="%POSTGRES_DB%" ^
  -e DATABASE_URL="%DATABASE_URL%" ^
  -e TEST_USERNAME="%TEST_USERNAME%" ^
  -e TEST_PASSWORD="%TEST_PASSWORD%" ^
  -e CI=true ^
  "%IMAGE_NAME%" ^
  -lc "sleep infinity"

if errorlevel 1 (
  echo Failed to start container
  exit /b 1
)

docker ps -a --filter "name=%CONTAINER_NAME%"

REM Ensure target dirs exist
docker exec "%CONTAINER_NAME%" sh -lc "mkdir -p /app/tests/GUI /app/tests/API /app/startup /app/resources"

echo.
echo Copying GUI tests...
if exist "Tests\GUI" docker cp "Tests\GUI\." "%CONTAINER_NAME%:/app/tests/GUI/"

echo Copying API tests...
if exist "Tests\API" docker cp "Tests\API\." "%CONTAINER_NAME%:/app/tests/API/"

echo Copying resources...
if exist "Tests\Resources" docker cp "Tests\Resources\." "%CONTAINER_NAME%:/app/resources/"

echo Copying startup scripts...
if exist "Tests\Startup" (
  docker cp "Tests\Startup\." "%CONTAINER_NAME%:/app/startup/"
  docker exec "%CONTAINER_NAME%" sh -lc "chmod +x /app/startup/*.sh 2>/dev/null || true"
)

echo.
echo [DEBUG] Container files after copy:
docker exec "%CONTAINER_NAME%" sh -lc "echo '--- /app/startup ---'; ls -la /app/startup; echo '--- /app/startup/*.sh ---'; ls -la /app/startup/*.sh 2>/dev/null || true; echo '--- /app/tests/GUI ---'; ls -la /app/tests/GUI 2>/dev/null || true; echo '--- /app/tests/API ---'; ls -la /app/tests/API 2>/dev/null || true"

echo.
echo Running tests (entrypoint)...
docker exec "%CONTAINER_NAME%" sh -lc "export PAUSE_BEFORE_TESTS=false; /app/entrypoint.sh"
set EXIT_CODE=%ERRORLEVEL%
echo [DEBUG] Test run exit code: %EXIT_CODE%

set "RESULTS_DIR=%~dp0test-results"

echo Copying test results...
if exist "%RESULTS_DIR%" rmdir /s /q "%RESULTS_DIR%" >nul 2>&1
docker cp "%CONTAINER_NAME%:/app/tests/test-results" "%RESULTS_DIR%" 2>nul || echo No test results to copy

echo ==========================================
echo Container logs:
echo ==========================================
docker logs "%CONTAINER_NAME%"

echo Cleaning up...
docker rm -f "%CONTAINER_NAME%" >nul 2>&1

echo ==========================================
if "%EXIT_CODE%"=="0" (
  echo Tests passed successfully
) else (
  echo Tests failed with exit code: %EXIT_CODE%
)
echo ==========================================

exit /b %EXIT_CODE%