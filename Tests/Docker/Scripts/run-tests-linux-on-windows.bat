@echo off
REM Run tests using Linux containers on Windows
REM This script is meant to be used from application repositories (e.g., Lisa) on Windows
REM It runs Linux containers via Docker Desktop (WSL 2 backend)
REM 
REM Usage:
REM   1. Ensure Docker Desktop is in Linux containers mode
REM   2. Build ImtCore Docker image (using build-docker-linux-on-windows.bat)
REM   3. Run this script from your application repository
REM   4. Tests will be executed automatically in the container

setlocal enabledelayedexpansion

REM Configuration
set CONTAINER_NAME=myapp-tests-%RANDOM%%RANDOM%
if "%IMAGE_NAME%"=="" set IMAGE_NAME=imtcore-tests:linux
if "%BASE_URL%"=="" set BASE_URL=http://host.docker.internal:3000
if "%START_POSTGRESQL%"=="" set START_POSTGRESQL=false
if "%POSTGRES_DB%"=="" set POSTGRES_DB=
if "%DATABASE_URL%"=="" set DATABASE_URL=
if "%TEST_USERNAME%"=="" set TEST_USERNAME=
if "%TEST_PASSWORD%"=="" set TEST_PASSWORD=

echo ==========================================
echo Running Linux containers on Windows
echo ==========================================
echo.

REM Check if Docker is installed
where docker >nul 2>&1
if errorlevel 1 (
    echo [91mError: Docker is not installed or not in PATH[0m
    exit /b 1
)

REM Check Docker mode
echo Checking Docker Desktop mode...
for /f "delims=" %%i in ('docker info --format "{{.OSType}}" 2^>nul') do set OS_TYPE=%%i

if not "!OS_TYPE!"=="linux" (
    echo.
    echo [93m Warning: Docker Desktop is in Windows containers mode![0m
    echo.
    echo To run Linux containers on Windows:
    echo   1. Right-click Docker Desktop icon in system tray
    echo   2. Select 'Switch to Linux containers...'
    echo   3. Wait for Docker to restart
    echo   4. Run this script again
    echo.
    exit /b 1
)

echo [92m✓ Docker Desktop is in Linux containers mode[0m
echo.

REM Check if image exists
docker image inspect "%IMAGE_NAME%" >nul 2>&1
if errorlevel 1 (
    echo [91mError: Docker image '%IMAGE_NAME%' not found[0m
    echo.
    echo Please build the image first using:
    echo   cd C:\path\to\ImtCore
    echo   .\Tests\Docker\build-docker-linux-on-windows.bat
    echo.
    exit /b 1
)

echo Starting Linux container: %CONTAINER_NAME%
echo Using image: %IMAGE_NAME%
echo.

REM Start the container in detached mode
REM Note: Use host.docker.internal to access Windows host from Linux container
docker run -d ^
  --name "%CONTAINER_NAME%" ^
  --add-host=host.docker.internal:host-gateway ^
  -e BASE_URL="%BASE_URL%" ^
  -e START_POSTGRESQL="%START_POSTGRESQL%" ^
  -e POSTGRES_DB="%POSTGRES_DB%" ^
  -e DATABASE_URL="%DATABASE_URL%" ^
  -e TEST_USERNAME="%TEST_USERNAME%" ^
  -e TEST_PASSWORD="%TEST_PASSWORD%" ^
  -e CI=true ^
  "%IMAGE_NAME%" ^
  sleep infinity

if errorlevel 1 (
    echo [91mFailed to start container[0m
    exit /b 1
)

REM Wait a bit for container to be ready
timeout /t 2 /nobreak >nul

REM Copy GUI tests (Playwright)
if exist "Tests\GUI" (
    dir /b "Tests\GUI" >nul 2>&1
    if not errorlevel 1 (
        echo Copying GUI tests...
        docker cp Tests/GUI/. "%CONTAINER_NAME%:/app/tests/GUI/"
    )
)

REM Copy API tests (Postman)
if exist "Tests\API" (
    dir /b "Tests\API" >nul 2>&1
    if not errorlevel 1 (
        echo Copying API tests...
        docker cp Tests/API/. "%CONTAINER_NAME%:/app/tests/API/"
    )
)

REM Copy resources (installers, SQL scripts, configs, etc.)
if exist "Tests\Docker\Resources" (
    dir /b "Tests\Docker\Resources" >nul 2>&1
    if not errorlevel 1 (
        echo Copying resources...
        docker cp Tests/Docker/Resources/. "%CONTAINER_NAME%:/app/custom-apps/resources/"
    )
)

REM Copy startup scripts
if exist "Tests\Docker\Startup" (
    dir /b "Tests\Docker\Startup" >nul 2>&1
    if not errorlevel 1 (
        echo Copying startup scripts...
        docker cp Tests/Docker/Startup/. "%CONTAINER_NAME%:/app/custom-apps/startup/"
        REM Make scripts executable
        docker exec "%CONTAINER_NAME%" chmod +x /app/custom-apps/startup/*.sh 2>nul
    )
)

REM Copy playwright.config.js if exists
if exist "Tests\playwright.config.js" (
    echo Copying playwright.config.js...
    docker cp Tests/playwright.config.js "%CONTAINER_NAME%:/app/tests/"
)

REM Copy package.json if exists (for additional dependencies)
if exist "Tests\package.json" (
    echo Copying package.json...
    docker cp Tests/package.json "%CONTAINER_NAME%:/app/tests/"
    echo Installing additional dependencies...
    docker exec "%CONTAINER_NAME%" npm install
)

REM Restart container to trigger automatic test detection and execution
echo Running tests...
docker restart "%CONTAINER_NAME%"

REM Wait for container to finish
docker wait "%CONTAINER_NAME%"

REM Get exit code
for /f "delims=" %%i in ('docker inspect "%CONTAINER_NAME%" --format="{{.State.ExitCode}}"') do set EXIT_CODE=%%i

REM Copy test results out
echo Copying test results...
docker cp "%CONTAINER_NAME%:/app/tests/test-results" ./test-results 2>nul || echo No test results to copy

REM Show logs
echo ==========================================
echo Container logs:
echo ==========================================
docker logs "%CONTAINER_NAME%"

REM Cleanup
echo Cleaning up...
docker stop "%CONTAINER_NAME%" >nul 2>&1
docker rm "%CONTAINER_NAME%" >nul 2>&1

echo ==========================================
if "%EXIT_CODE%"=="0" (
    echo [92m✓ Tests passed successfully![0m
    echo.
    echo Note: Linux container ran on Windows via Docker Desktop ^(WSL 2^)
) else (
    echo [91m✗ Tests failed with exit code: %EXIT_CODE%[0m
)
echo ==========================================

exit /b %EXIT_CODE%
