@echo off
REM Docker entrypoint script for Windows test environment
REM Handles startup sequence: PostgreSQL -> Custom Apps -> Tests

setlocal enabledelayedexpansion

echo ========================================
echo Starting Test Environment (Windows^)
echo ========================================

REM Step 1: Start PostgreSQL if needed
if "%START_POSTGRESQL%"=="true" (
    echo Step 1: Starting PostgreSQL...
    
    REM Find PostgreSQL installation
    for /d %%P in ("C:\Program Files\PostgreSQL\*") do set PG_PATH=%%P
    
    if defined PG_PATH (
        set PG_BIN=%PG_PATH%\bin
        set PG_DATA=C:\PostgreSQL\data
        set PG_LOG=C:\PostgreSQL
        
        REM Default password
        if not defined POSTGRES_PASSWORD set POSTGRES_PASSWORD=root
        
        REM Create log directory if needed
        if not exist "!PG_LOG!" mkdir "!PG_LOG!"
        
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
                echo Found stale postmaster.pid (pid=!OLD_PID!^). Removing...
                del "!PG_DATA!\postmaster.pid"
                echo Starting PostgreSQL...
                "!PG_BIN!\pg_ctl.exe" -D "!PG_DATA!" -l "!PG_LOG!\logfile.log" start
            ) else (
                echo PostgreSQL appears to be running already (pid=!OLD_PID!^). Skipping start.
            )
        ) else (
            echo Starting PostgreSQL...
            "!PG_BIN!\pg_ctl.exe" -D "!PG_DATA!" -l "!PG_LOG!\logfile.log" start
        )
        
        REM Wait for PostgreSQL
        echo Waiting for PostgreSQL to be ready...
        set /a ATTEMPTS=0
        :WAIT_PG
        set /a ATTEMPTS+=1
        if %ATTEMPTS% gtr 30 (
            echo [ERROR] PostgreSQL failed to start
            goto SKIP_PG
        )
        "!PG_BIN!\psql.exe" -U postgres -c "SELECT 1" >nul 2>&1
        if errorlevel 1 (
            echo   Attempt %ATTEMPTS%/30...
            timeout /t 2 /nobreak >nul
            goto WAIT_PG
        )
        echo [OK] PostgreSQL is ready
        
        REM Set password
        echo Setting postgres password (default^)
        "!PG_BIN!\psql.exe" -U postgres -c "ALTER USER postgres WITH PASSWORD '%POSTGRES_PASSWORD%';" >nul 2>&1
        
        REM Create test database if specified
        if defined POSTGRES_DB (
            echo Creating database: %POSTGRES_DB%
            "!PG_BIN!\psql.exe" -U postgres -c "CREATE DATABASE %POSTGRES_DB%" >nul 2>&1
        )
        
        echo [OK] PostgreSQL initialized
        :SKIP_PG
    ) else (
        echo [WARNING] PostgreSQL not found in C:\Program Files\PostgreSQL
        echo Continuing without PostgreSQL...
    )
) else (
    echo Step 1: PostgreSQL startup skipped (START_POSTGRESQL not set to true^)
)

REM Step 2: Run custom application installers and startup scripts
if exist "C:\app\startup" (
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
    echo   APP_DIR=%APP_DIR%
    echo   STARTUP_DIR=%STARTUP_DIR%
    echo   RESOURCES_DIR=%RESOURCES_DIR%
    echo   TESTS_DIR=%TESTS_DIR%
    
    REM Get all .bat scripts and sort them
    set SCRIPT_COUNT=0
    for %%F in (C:\app\startup\*.bat) do (
        set /a SCRIPT_COUNT+=1
    )
    
    if !SCRIPT_COUNT! equ 0 (
        echo No startup scripts found: C:\app\startup\*.bat
    ) else (
        echo Found !SCRIPT_COUNT! startup script(s^):
        for %%F in (C:\app\startup\*.bat) do echo   - %%~nxF
        
        echo Executing startup scripts in lexicographic order...
        for %%F in (C:\app\startup\*.bat) do (
            echo Running startup script: %%~nxF
            call "%%F"
            echo Finished: %%~nxF (exit=!ERRORLEVEL!^)
        )
    )
    
    echo [OK] Custom applications initialized
) else (
    echo Step 2: No custom applications to start (C:\app\startup not found^)
)

REM Step 3: Run tests or custom command
echo Step 3: Starting tests...
echo ========================================

cd /d C:\app\tests

if "%PAUSE_BEFORE_TESTS%"=="true" (
    echo PAUSE_BEFORE_TESTS=true -^> pausing before running tests.
    echo Attach with: docker exec -it ^<container^> cmd
    timeout /t 3600 /nobreak >nul
)

REM Auto-detect tests
echo Auto-detecting tests...

set GUI_TESTS_FOUND=0
set API_TESTS_FOUND=0

REM Check for GUI tests (Playwright^)
if exist "C:\app\tests\GUI" (
    dir /s /b "C:\app\tests\GUI\*.spec.js" "C:\app\tests\GUI\*.spec.ts" "C:\app\tests\GUI\*.test.js" "C:\app\tests\GUI\*.test.ts" 2>nul | findstr . >nul
    if not errorlevel 1 (
        set GUI_TESTS_FOUND=1
        echo [OK] Found Playwright tests in GUI folder
    )
)

REM Check for API tests (Postman/Newman^)
if exist "C:\app\tests\API" (
    dir /s /b "C:\app\tests\API\*collection*.json" 2>nul | findstr . >nul
    if not errorlevel 1 (
        set API_TESTS_FOUND=1
        echo [OK] Found Postman collections in API folder
    )
)

if %GUI_TESTS_FOUND% equ 1 (
    set EXIT_CODE=0
    
    echo Preparing Playwright dependencies...
    if exist "C:\app\tests\package.json" (
        if exist "C:\app\tests\package-lock.json" (
            cd /d C:\app\tests
            call npm ci
            if errorlevel 1 set EXIT_CODE=!ERRORLEVEL!
        ) else (
            cd /d C:\app\tests
            call npm install
            if errorlevel 1 set EXIT_CODE=!ERRORLEVEL!
        )
    ) else (
        echo No C:\app\tests\package.json found. Cannot install @playwright/test.
        set EXIT_CODE=1
    )
    
    if !EXIT_CODE! equ 0 (
        echo Running Playwright tests...
        cd /d C:\app\tests
        
        REM Build playwright command with optional --update-snapshots flag
        set PLAYWRIGHT_CMD=npx playwright test
        if "%UPDATE_SNAPSHOTS%"=="true" (
            echo UPDATE_SNAPSHOTS=true - updating reference screenshots
            set PLAYWRIGHT_CMD=!PLAYWRIGHT_CMD! --update-snapshots
        )
        
        call !PLAYWRIGHT_CMD!
        if errorlevel 1 set EXIT_CODE=!ERRORLEVEL!
    )
    
    exit /b !EXIT_CODE!
)

if %API_TESTS_FOUND% equ 1 (
    set EXIT_CODE=0
    
    echo Running Postman tests...
    
    REM Use first environment file if exists (optional^)
    for /r "C:\app\tests\API" %%F in (*environment*.json) do (
        set ENV_FILE=%%F
        goto :FOUND_ENV
    )
    :FOUND_ENV
    
    if defined ENV_FILE (
        echo Using environment: %ENV_FILE%
        for %%C in (C:\app\tests\API\*collection*.json) do (
            echo Running collection: %%~nxC
            call newman run "%%C" -e "%ENV_FILE%"
            if errorlevel 1 set EXIT_CODE=!ERRORLEVEL!
        )
    ) else (
        for %%C in (C:\app\tests\API\*collection*.json) do (
            echo Running collection: %%~nxC
            call newman run "%%C"
            if errorlevel 1 set EXIT_CODE=!ERRORLEVEL!
        )
    )
    
    exit /b !EXIT_CODE!
)

if %GUI_TESTS_FOUND% equ 0 if %API_TESTS_FOUND% equ 0 (
    echo No tests found in GUI or API folders
    echo To run tests:
    echo   - Copy Playwright tests to C:\app\tests\GUI\
    echo   - Copy Postman collections to C:\app\tests\API\
    exit /b 0
)
