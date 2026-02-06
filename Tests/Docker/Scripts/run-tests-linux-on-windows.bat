@echo off
REM Wrapper script - calls the core script with default/example configuration
REM For production use in application repos, copy run-tests-linux-on-windows-template.bat instead

REM Set defaults if not already set
if "%IMAGE_NAME%"=="" set IMAGE_NAME=imtcore-tests:linux
if "%BASE_URL%"=="" set BASE_URL=
if "%START_POSTGRESQL%"=="" set START_POSTGRESQL=true

REM Call the core script from the same directory
call "%~dp0run-tests-linux-on-windows-core.bat"
exit /b %ERRORLEVEL%