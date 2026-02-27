@echo off
REM Simple configuration file for running Docker tests (Linux containers on Windows)
REM Copy this file to your application's Tests directory and configure the variables below

REM ==========================================
REM CONFIGURATION
REM ==========================================

REM Application URL (use host.docker.internal to access Windows host)
set BASE_URL=http://host.docker.internal:7776

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
  for %%I in ("%SCRIPT_DIR%..") do set APP_ROOT=%%~fI
  REM Try common relative locations (same level or higher)
  for %%I in ("%APP_ROOT%\..\ImtCore" "%APP_ROOT%\..\..\ImtCore" "%APP_ROOT%\..\..\..\ImtCore") do (
    if not defined IMTCOREDIR if exist "%%~fI\Tests\Docker\Scripts\run-tests-linux-on-windows-core.bat" set IMTCOREDIR=%%~fI
  )
  if not defined IMTCOREDIR (
    echo ERROR: IMTCOREDIR environment variable is not set and ImtCore not found at expected location.
    echo Please either:
    echo   1. Set IMTCOREDIR environment variable: set IMTCOREDIR=C:\path\to\ImtCore
    echo   2. Place ImtCore at the same level as your application directory
    exit /b 1
  )
)

REM Validate ImtCore path
if not exist "%IMTCOREDIR%\Tests\Docker\Scripts\run-tests-linux-on-windows-core.bat" (
  echo ERROR: Cannot find core script at: %IMTCOREDIR%\Tests\Docker\Scripts\run-tests-linux-on-windows-core.bat
  echo Please ensure IMTCOREDIR points to a valid ImtCore directory.
  exit /b 1
)

call "%IMTCOREDIR%\Tests\Docker\Scripts\run-tests-linux-on-windows-core.bat"
exit /b %ERRORLEVEL%
