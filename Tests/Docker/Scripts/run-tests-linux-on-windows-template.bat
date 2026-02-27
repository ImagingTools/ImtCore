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
  REM If IMTCOREDIR is not set, assume ImtCore is at ..\..\ImtCore
  set "IMTCOREDIR=%~dp0..\..\ImtCore"
)

REM Validate ImtCore path
if not exist "%IMTCOREDIR%\Tests\Docker\Scripts\run-tests-linux-on-windows-core.bat" (
  echo ERROR: Cannot find core script at: %IMTCOREDIR%\Tests\Docker\Scripts\run-tests-linux-on-windows-core.bat
  echo Please ensure IMTCOREDIR points to a valid ImtCore directory.
  exit /b 1
)

call "%IMTCOREDIR%\Tests\Docker\Scripts\run-tests-linux-on-windows-core.bat"
exit /b %ERRORLEVEL%
