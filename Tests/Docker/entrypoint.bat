@echo off
REM Docker entrypoint script for Windows test environment
REM Handles startup sequence: PostgreSQL -> Custom Apps -> Tests

setlocal enabledelayedexpansion

echo ========================================
echo Starting Test Environment (Windows)
echo ========================================

REM Add system paths to PATH (for ping, timeout, powershell, etc.)
set "PATH=C:\Windows\System32;C:\Windows\System32\WindowsPowerShell\v1.0;C:\Windows;!PATH!"

REM Add Node.js to PATH
if exist "C:\Program Files\nodejs" set "PATH=C:\Program Files\nodejs;!PATH!"

REM Add pre-installed node_modules binaries to PATH
if exist "C:\modules\node_modules\.bin" set "PATH=C:\modules\node_modules\.bin;!PATH!"

REM Set NODE_PATH to include global modules so scripts can find them
REM We will append specific test directories to this later
set "NODE_PATH=C:\modules\node_modules"

REM Add PostgreSQL to PATH if found (needed for startup scripts even if not starting PG)
for /d %%P in ("C:\Program Files\PostgreSQL\*") do (
    set "PATH=%%P\bin;!PATH!"
    set "PG_BIN=%%P\bin"
)

REM Step 1: Start PostgreSQL if needed
if not "!START_POSTGRESQL!"=="true" goto SKIP_POSTGRESQL

echo Step 1: Starting PostgreSQL...

REM Find PostgreSQL installation
set PG_PATH=
for /d %%P in ("C:\Program Files\PostgreSQL\*") do set PG_PATH=%%P

if not defined PG_PATH (
    echo [WARNING] PostgreSQL not found in C:\Program Files\PostgreSQL
    echo Continuing without PostgreSQL...
    goto SKIP_POSTGRESQL
)

set PG_BIN=!PG_PATH!\bin
set PG_DATA=C:\PostgreSQL\data
set PG_LOG=C:\PostgreSQL\logfile.log

REM Default password
if not defined POSTGRES_PASSWORD set POSTGRES_PASSWORD=root

REM Create log directory if needed
if not exist "C:\PostgreSQL" mkdir "C:\PostgreSQL"

REM Ensure data dir exists
if not exist "!PG_DATA!" (
    echo Initializing PostgreSQL data directory...
    mkdir "!PG_DATA!"
    "!PG_BIN!\initdb.exe" -D "!PG_DATA!" -U postgres --auth-host=scram-sha-256
)

REM Check for stale postmaster.pid
if exist "!PG_DATA!\postmaster.pid" (
    set /p OLD_PID=<"!PG_DATA!\postmaster.pid"
    tasklist /FI "PID eq !OLD_PID!" 2>nul | find "!OLD_PID!" >nul
    if errorlevel 1 (
        echo Found stale postmaster.pid. Removing...
        del "!PG_DATA!\postmaster.pid"
    ) else (
        echo PostgreSQL appears to be running already. Skipping start.
        goto WAIT_PG
    )
)

echo Starting PostgreSQL...
"!PG_BIN!\pg_ctl.exe" -D "!PG_DATA!" -l "!PG_LOG!" start -w -t 60

if errorlevel 1 (
    echo [ERROR] PostgreSQL failed to start. Log contents:
    echo ----------------------------------------
    if exist "!PG_LOG!" (
        type "!PG_LOG!"
    ) else (
        echo Log file not found: !PG_LOG!
    )
    echo ----------------------------------------
    goto SKIP_POSTGRESQL
)

REM Wait for PostgreSQL
:WAIT_PG
echo Waiting for PostgreSQL to be ready...
set ATTEMPTS=0

:WAIT_PG_LOOP
set /a ATTEMPTS+=1
if !ATTEMPTS! gtr 30 (
    echo [ERROR] PostgreSQL failed to start after 30 attempts
    echo Log contents:
    echo ----------------------------------------
    if exist "!PG_LOG!" type "!PG_LOG!"
    echo ----------------------------------------
    goto SKIP_POSTGRESQL
)
"!PG_BIN!\psql.exe" -U postgres -c "SELECT 1" >nul 2>&1
if errorlevel 1 (
    echo   Attempt !ATTEMPTS!/30...
    timeout /t 2 /nobreak >nul
    goto WAIT_PG_LOOP
)
echo [OK] PostgreSQL is ready

REM Set password
echo Setting postgres password
"!PG_BIN!\psql.exe" -U postgres -c "ALTER USER postgres WITH PASSWORD '!POSTGRES_PASSWORD!';" >nul 2>&1

REM NOTE:
REM Removed automatic CREATE DATABASE logic because it can fail or require interactive auth
REM depending on container/pg_hba configuration.
REM If you need DB creation, add a custom startup script in C:\app\startup.
if defined POSTGRES_DB (
    echo [INFO] POSTGRES_DB is set to !POSTGRES_DB!, but automatic database creation is disabled in entrypoint.bat
    echo [INFO] Create the database in a custom startup script under C:\app\startup if needed.
)

echo [OK] PostgreSQL initialized
goto AFTER_POSTGRESQL

:SKIP_POSTGRESQL
if not "!START_POSTGRESQL!"=="true" (
    echo Step 1: PostgreSQL startup skipped
)

:AFTER_POSTGRESQL

REM Step 2: Run custom application installers and startup scripts
if not exist "C:\app\startup" goto SKIP_STARTUP

echo Step 2: Running custom application scripts...
echo Startup dir: C:\app\startup

echo Listing C:\app\startup:
dir /b "C:\app\startup" 2>nul

REM Export path environment variables for startup scripts
set APP_DIR=C:\app
set STARTUP_DIR=C:\app\startup
set RESOURCES_DIR=C:\app\resources
set TESTS_DIR=C:\app\tests

echo Environment variables set for startup scripts:
echo   APP_DIR=!APP_DIR!
echo   STARTUP_DIR=!STARTUP_DIR!
echo   RESOURCES_DIR=!RESOURCES_DIR!
echo   TESTS_DIR=!TESTS_DIR!

REM Count scripts
set SCRIPT_COUNT=0
for %%F in ("C:\app\startup\*.bat") do set /a SCRIPT_COUNT+=1

if !SCRIPT_COUNT! equ 0 (
    echo No startup scripts found
) else (
    echo Found !SCRIPT_COUNT! startup scripts
    for %%F in ("C:\app\startup\*.bat") do (
        echo Running startup script: %%~nxF
        call "%%F"
        echo Finished: %%~nxF
    )
)

echo [OK] Custom applications initialized
goto AFTER_STARTUP

:SKIP_STARTUP
echo Step 2: No custom applications to start

:AFTER_STARTUP

REM Step 3: Run tests or custom command
echo Step 3: Starting tests...
echo ========================================

cd /d C:\app\tests

if "!PAUSE_BEFORE_TESTS!"=="true" (
    echo PAUSE_BEFORE_TESTS=true - pausing before running tests.
    echo Attach with: docker exec -it [container] cmd
    timeout /t 3600 /nobreak >nul
)

REM Auto-detect tests
echo Auto-detecting tests...

set GUI_TESTS_FOUND=0
set API_TESTS_FOUND=0

REM Check for GUI tests (Playwright)
if exist "C:\app\tests\GUI" (
    dir /s /b "C:\app\tests\GUI\*.spec.js" "C:\app\tests\GUI\*.spec.ts" "C:\app\tests\GUI\*.test.js" "C:\app\tests\GUI\*.test.ts" 2>nul | findstr . >nul
    if not errorlevel 1 (
        set GUI_TESTS_FOUND=1
        echo [OK] Found Playwright tests in GUI folder
    )
)

REM Check for API tests (Postman/Newman)
if exist "C:\app\tests\API" (
    dir /s /b "C:\app\tests\API\*collection*.json" 2>nul | findstr . >nul
    if not errorlevel 1 (
        set API_TESTS_FOUND=1
        echo [OK] Found Postman collections in API folder
    )
)

REM Run GUI tests
if !GUI_TESTS_FOUND! equ 0 goto SKIP_GUI_TESTS

set EXIT_CODE=0

REM Verify pre-installed Playwright exists
if not exist "C:\modules\node_modules\@playwright\test" (
    echo [ERROR] @playwright/test not found in C:\modules\node_modules
    echo [ERROR] The Docker image may not have been built correctly.
    set EXIT_CODE=1
    goto END_TESTS
)

echo [OK] Using pre-installed Playwright from C:\modules

echo Running Playwright tests...
cd /d C:\app\tests\GUI

REM Allow require('utils') by pointing NODE_PATH to both global modules AND GUI folder
set "NODE_PATH=C:\modules\node_modules;C:\app\tests\GUI"

REM Use Playwright binary from C:\modules explicitly
set "PLAYWRIGHT_BIN=C:\modules\node_modules\.bin\playwright.cmd"
if not exist "!PLAYWRIGHT_BIN!" (
    echo [ERROR] Playwright CLI not found: !PLAYWRIGHT_BIN!
    set EXIT_CODE=1
    goto END_TESTS
)

REM Define output variables for reports
REM This forces reports to go to the mounted volume C:\app\tests\test-results
REM instead of the default folder inside GUI (which is mounted from ImtCore)
set "PLAYWRIGHT_HTML_REPORT=C:\app\tests\test-results\playwright-report"
set "PLAYWRIGHT_JSON_OUTPUT_FILE=C:\app\tests\test-results\playwright-results.json"
set "PLAYWRIGHT_JUNIT_OUTPUT_FILE=C:\app\tests\test-results\playwright-junit.xml"

REM Pass these as env vars is not enough because playwright.config.js might ignore them
REM so we also pass them as command line arguments where possible, or rely on config to read envs.
REM The standard playwright.config.js in ImtCore DOES read env vars if configured,
REM but let's override via CLI arguments to be sure.

set "PLAYWRIGHT_CMD=!PLAYWRIGHT_BIN! test --reporter=list,html,json,junit"

REM Note: Playwright CLI doesn't easily accept output paths for specific reporters via flags mixed with comma-separated list.
REM It relies on env vars usually. So we export them.

if "!UPDATE_SNAPSHOTS!"=="true" (
    echo UPDATE_SNAPSHOTS=true - updating reference screenshots
    set "PLAYWRIGHT_CMD=!PLAYWRIGHT_CMD! --update-snapshots"
)

echo Executing: !PLAYWRIGHT_CMD!
echo Reports will be saved to: C:\app\tests\test-results

call !PLAYWRIGHT_CMD!
if errorlevel 1 set EXIT_CODE=!ERRORLEVEL!

goto END_TESTS

:SKIP_GUI_TESTS

REM Run API tests
if !API_TESTS_FOUND! equ 0 goto SKIP_API_TESTS

set EXIT_CODE=0

echo Running Postman tests...

REM Find environment file
set ENV_FILE=
for /r "C:\app\tests\API" %%F in (*environment*.json) do (
    set "ENV_FILE=%%F"
    goto RUN_NEWMAN
)

:RUN_NEWMAN
if defined ENV_FILE (
    echo Using environment: !ENV_FILE!
    for %%C in ("C:\app\tests\API\*collection*.json") do (
        echo Running collection: %%~nxC
        call newman run "%%C" -e "!ENV_FILE!"
        if errorlevel 1 set EXIT_CODE=!ERRORLEVEL!
    )
) else (
    for %%C in ("C:\app\tests\API\*collection*.json") do (
        echo Running collection: %%~nxC
        call newman run "%%C"
        if errorlevel 1 set EXIT_CODE=!ERRORLEVEL!
    )
)

goto END_TESTS

:SKIP_API_TESTS

echo No tests found in GUI or API folders
echo To run tests:
echo   - Copy Playwright tests to C:\app\tests\GUI\
echo   - Copy Postman collections to C:\app\tests\API\
set EXIT_CODE=0

:END_TESTS
exit /b !EXIT_CODE!
