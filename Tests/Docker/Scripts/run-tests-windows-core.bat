@echo off
setlocal enabledelayedexpansion

REM --- SETUP ---
REM Generate unique container name
if not defined CONTAINER_NAME (
    for /f "tokens=2-4 delims=/ " %%a in ('date /t') do set mydate=%%c%%a%%b
    for /f "tokens=1-2 delims=/:" %%a in ('time /t') do set mytime=%%a%%b
    set mytime=!mytime: =0!
    set CONTAINER_NAME=imtcore-tests-!mydate!!mytime!
)

set IMAGE_NAME=imtcore-tests:windows
if not defined POSTGRES_PASSWORD set POSTGRES_PASSWORD=root
if not defined POSTGRES_DB set POSTGRES_DB=test_db
if defined POSTGRES_DB set DATABASE_URL=postgresql://postgres:%POSTGRES_PASSWORD%@localhost:5432/%POSTGRES_DB%

echo ==========================================
echo Running Windows containers on Windows
echo ==========================================

REM Check Docker
docker --version >nul 2>&1
if errorlevel 1 exit /b 1
docker image inspect %IMAGE_NAME% >nul 2>&1
if errorlevel 1 exit /b 1

REM --- PATH LOGIC ---
REM 1. Determine ImtCore location (Where files actually appear due to volume mount)
set "IMTCORE_DOCKER_DIR=%~dp0.."
for %%i in ("%IMTCORE_DOCKER_DIR%") do set IMTCORE_DOCKER_DIR=%%~fi

REM The "Hidden" locations where artifacts are created
set "GHOST_RESULTS=%IMTCORE_DOCKER_DIR%\GUI\test-results"
set "GHOST_REPORT=%IMTCORE_DOCKER_DIR%\GUI\playwright-report"

REM 2. Determine Destination location (Where we want them)
echo %CD% | findstr /I /C:"\Tests" >nul
if %errorlevel% equ 0 (
    set "DEST_RESULTS_DIR=%CD%\test-results"
) else (
    set "DEST_RESULTS_DIR=%CD%\Tests\test-results"
)

echo [DEBUG] ImtCore Dir:     %IMTCORE_DOCKER_DIR%
echo [DEBUG] Ghost Results:   %GHOST_RESULTS%
echo [DEBUG] Target Results:  %DEST_RESULTS_DIR%

echo [DEBUG] Cleaning up old results in Target...
if exist "%DEST_RESULTS_DIR%" rmdir /S /Q "%DEST_RESULTS_DIR%"
mkdir "%DEST_RESULTS_DIR%"

REM Clean Source (in ImtCore) to ensure fresh run
if exist "%GHOST_RESULTS%" rmdir /S /Q "%GHOST_RESULTS%"
if exist "%GHOST_REPORT%" rmdir /S /Q "%GHOST_REPORT%"

REM --- DOCKER EXECUTION ---
set VOLUME_MOUNTS=
REM Mount ImtCore (This causes the side-effect of writing to host)
set VOLUME_MOUNTS=%VOLUME_MOUNTS% -v "%IMTCORE_DOCKER_DIR%:C:\app\imtcore"
if exist "%IMTCORE_DOCKER_DIR%\GUI" set VOLUME_MOUNTS=%VOLUME_MOUNTS% -v "%IMTCORE_DOCKER_DIR%\GUI:C:\app\tests\GUI"

REM Mount Application Tests
if exist "%CD%\Tests\GUI" (
    set VOLUME_MOUNTS=%VOLUME_MOUNTS% -v "%CD%\Tests\GUI:C:\app\tests\GUI\app"
) else if exist "%CD%\GUI" (
    set VOLUME_MOUNTS=%VOLUME_MOUNTS% -v "%CD%\GUI:C:\app\tests\GUI\app"
)

if exist "%CD%\Tests\API" set VOLUME_MOUNTS=%VOLUME_MOUNTS% -v "%CD%\Tests\API:C:\app\tests\API"
if exist "%CD%\Tests\Startup" set VOLUME_MOUNTS=%VOLUME_MOUNTS% -v "%CD%\Tests\Startup:C:\app\startup"
if exist "%CD%\Tests\Resources" set VOLUME_MOUNTS=%VOLUME_MOUNTS% -v "%CD%\Tests\Resources:C:\app\resources"

echo Starting container...
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

if errorlevel 1 exit /b 1

echo Running tests...
docker exec %CONTAINER_NAME% cmd /c "set PAUSE_BEFORE_TESTS=false && C:\app\imtcore\entrypoint.bat"
set EXIT_CODE=%ERRORLEVEL%

echo.
echo [DEBUG] Moving artifacts from ImtCore folder to Target...

REM 1. Move 'test-results' (Screenshots, JSON, XML)
if exist "%GHOST_RESULTS%" (
    echo Found results in ImtCore folder. Moving...
    xcopy /E /I /Y /Q "%GHOST_RESULTS%" "%DEST_RESULTS_DIR%" >nul
    REM Remove from ImtCore to keep it clean
    rmdir /S /Q "%GHOST_RESULTS%"
) else (
    echo [WARN] No results found in %GHOST_RESULTS%
)

REM 2. Move 'playwright-report' (HTML)
if exist "%GHOST_REPORT%" (
    echo Found report in ImtCore folder. Moving...
    mkdir "%DEST_RESULTS_DIR%\playwright-report"
    xcopy /E /I /Y /Q "%GHOST_REPORT%" "%DEST_RESULTS_DIR%\playwright-report" >nul
    REM Remove from ImtCore to keep it clean
    rmdir /S /Q "%GHOST_REPORT%"
)

echo Cleaning up container...
docker rm -f %CONTAINER_NAME% >nul 2>&1

echo ==========================================
if %EXIT_CODE% equ 0 (
    echo Tests PASSED.
) else (
    echo Tests FAILED.
)
echo Results are here: %DEST_RESULTS_DIR%
echo ==========================================

exit /b %EXIT_CODE%