@echo off
REM Simple configuration file for running Docker tests (Windows containers on Windows)
REM Copy this file to your application's Tests directory and configure the variables below

REM ==========================================
REM CONFIGURATION
REM ==========================================

REM Application URL (use localhost for native Windows containers)
set BASE_URL=http://localhost:8080

REM PostgreSQL password (optional, default: "root" if not set)
REM Uncomment and set if you need a different password:
REM set POSTGRES_PASSWORD=your_password

REM Test credentials - supports multiple users
REM Format: "username1:password1,username2:password2,username3:password3"
REM Or simple single user (backward compatible):
set TEST_USERS=test@example.com:testpassword
REM Examples:
REM Single user: set TEST_USERS=admin@example.com:admin123
REM Multiple users: set TEST_USERS=admin@example.com:admin123,user@example.com:user123,viewer@example.com:viewer123

REM Update Playwright reference screenshots (optional, default: false)
REM Set to "true" to update all reference screenshots (visual regression baselines)
REM Uncomment to enable:
REM set UPDATE_SNAPSHOTS=true

REM ==========================================
REM DO NOT MODIFY BELOW THIS LINE
REM ==========================================

REM Determine ImtCore directory
if not defined IMTCOREDIR (
    REM If IMTCOREDIR is not set, assume ImtCore is at the same level as the application
    set SCRIPT_DIR=%~dp0
    REM Go up to application root (assuming script is in App\Tests\)
    for %%A in ("%SCRIPT_DIR%..") do set APP_ROOT=%%~fA
    REM Assume ImtCore is at same level as application
    for %%A in ("%APP_ROOT%..") do set PARENT_DIR=%%~fA
    set IMTCORE_CANDIDATE=%PARENT_DIR%\ImtCore
    
    if exist "%IMTCORE_CANDIDATE%\Tests\Docker\Scripts\run-tests-windows-core.bat" (
        set IMTCOREDIR=%IMTCORE_CANDIDATE%
    ) else (
        echo ERROR: IMTCOREDIR environment variable is not set and ImtCore not found at expected location.
        echo Please either:
        echo   1. Set IMTCOREDIR environment variable: set IMTCOREDIR=C:\path\to\ImtCore
        echo   2. Place ImtCore at the same level as your application directory
        exit /b 1
    )
)

REM Validate ImtCore path
set CORE_SCRIPT_PATH=%IMTCOREDIR%\Tests\Docker\Scripts\run-tests-windows-core.bat
if not exist "%CORE_SCRIPT_PATH%" (
    echo ERROR: Cannot find core script at: %CORE_SCRIPT_PATH%
    echo Please ensure IMTCOREDIR points to a valid ImtCore directory.
    exit /b 1
)

call "%CORE_SCRIPT_PATH%"
exit /b %ERRORLEVEL%
