@echo off
setlocal EnableDelayedExpansion

REM ===============================================================
REM Configures and builds a CMake project with Ninja. Used by the
REM TeamCity release pipelines to avoid repeating the same
REM mkdir/cmake/build block for every product.
REM
REM Usage:
REM   BuildProject.bat <projectRootDir> <buildDirName> <buildType> [extraCmakeArgs...]
REM
REM   projectRootDir  Path to the repo root; expects Build\CMake\CMakeLists.txt
REM   buildDirName    Name of the build subdirectory under Build\CMake
REM   buildType       CMAKE_BUILD_TYPE value, e.g. Release or Debug
REM   extraCmakeArgs  Any additional -D flags to pass to the cmake configure step
REM ===============================================================

set "PROJECT_ROOT_DIR=%~1"
set "BUILD_DIR_NAME=%~2"
set "BUILD_TYPE=%~3"

if "%PROJECT_ROOT_DIR%"=="" (
    echo ERROR: BuildProject.bat requires a project root directory as the first argument
    exit /b 1
)
if "%BUILD_DIR_NAME%"=="" (
    echo ERROR: BuildProject.bat requires a build directory name as the second argument
    exit /b 1
)
if "%BUILD_TYPE%"=="" (
    echo ERROR: BuildProject.bat requires a build type as the third argument
    exit /b 1
)

set "EXTRA_ARGS="
shift
shift
shift
:collect_args
if "%~1"=="" goto :args_done
set "EXTRA_ARGS=!EXTRA_ARGS! %~1"
shift
goto :collect_args
:args_done

cd /d "%PROJECT_ROOT_DIR%\Build\CMake" || exit /b 1

if not exist "CMakeLists.txt" (
    echo ERROR: CMakeLists.txt not found in %PROJECT_ROOT_DIR%\Build\CMake
    exit /b 1
)

if not exist "%BUILD_DIR_NAME%" mkdir "%BUILD_DIR_NAME%"
cd /d "%BUILD_DIR_NAME%" || exit /b 1

echo Configuring %PROJECT_ROOT_DIR% (%BUILD_TYPE%)...
cmake -G Ninja -DCMAKE_BUILD_TYPE=%BUILD_TYPE%!EXTRA_ARGS! ..
if errorlevel 1 exit /b 1

echo Building %PROJECT_ROOT_DIR% (%BUILD_TYPE%)...
cmake --build .
if errorlevel 1 exit /b 1

endlocal
exit /b 0
