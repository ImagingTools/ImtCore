@echo off
REM Wrapper script - calls the core script with default/example configuration
REM For production use in application repos, copy run-tests-windows-template.bat instead

REM Set defaults if not already set
if not defined BASE_URL set BASE_URL=http://localhost:8080
if not defined POSTGRES_DB set POSTGRES_DB=test_db
if not defined POSTGRES_PASSWORD set POSTGRES_PASSWORD=root

REM Call the core script from the same directory
set SCRIPT_DIR=%~dp0
call "%SCRIPT_DIR%run-tests-windows-core.bat"
exit /b %ERRORLEVEL%
