@echo off
REM Wrapper script - calls the core script with default/example configuration
REM For production use in application repos, copy run-tests-linux-on-windows-template.bat instead

REM Set defaults if not already set
if "%BASE_URL%"=="" set BASE_URL=http://host.docker.internal:8080
if "%POSTGRES_DB%"=="" set POSTGRES_DB=test_db
if "%POSTGRES_PASSWORD%"=="" set POSTGRES_PASSWORD=root

REM Call the core script from the same directory
call "%~dp0run-tests-linux-on-windows-core.bat"
exit /b %ERRORLEVEL%