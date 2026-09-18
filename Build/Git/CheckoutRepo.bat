@echo off
setlocal EnableDelayedExpansion

REM ===============================================================
REM Checks out a sibling repository to a specific version and
REM updates its submodules. Used by the TeamCity release pipelines
REM to avoid repeating the same block for every dependency repo.
REM
REM Usage:
REM   CheckoutRepo.bat <repoDir> <versionValue> [postCheckoutScript]
REM
REM   repoDir            Path to the already-cloned repository working dir
REM   versionValue       Tag, branch, or commit hash to switch to.
REM                       If empty, checks out "main" and hard-resets to
REM                       origin/main.
REM   postCheckoutScript Relative path (from repoDir) to a script to call
REM                       if it exists, e.g. Build\Git\UpdateVersion.bat
REM                       (default) or Build\Git\GenerateVersion.bat.
REM ===============================================================

set "REPO_DIR=%~1"
set "VERSION_VALUE=%~2"
set "POST_SCRIPT=%~3"
if "%POST_SCRIPT%"=="" set "POST_SCRIPT=Build\Git\UpdateVersion.bat"

if "%REPO_DIR%"=="" (
    echo ERROR: CheckoutRepo.bat requires a repository directory as the first argument
    exit /b 1
)

pushd "%REPO_DIR%" || exit /b 1

git fetch origin --tags
if errorlevel 1 goto :fail

if NOT "%VERSION_VALUE%"=="" (
    echo Switching to %VERSION_VALUE%

    git show-ref --verify --quiet refs/tags/%VERSION_VALUE%
    if !errorlevel! == 0 (
        echo Detected tag
        git checkout "%VERSION_VALUE%"
    ) else (
        git show-ref --verify --quiet refs/remotes/origin/%VERSION_VALUE%
        if !errorlevel! == 0 (
            echo Detected branch
            git checkout -B "%VERSION_VALUE%" "origin/%VERSION_VALUE%"
        ) else (
            echo Assuming commit hash
            git checkout "%VERSION_VALUE%"
        )
    )
) else (
    git checkout main
    git reset --hard origin/main
)
if errorlevel 1 goto :fail

git lfs pull
if errorlevel 1 goto :fail

echo Updating submodules...
git submodule sync --recursive
git submodule update --init --recursive --force
git submodule foreach --recursive git lfs pull
if errorlevel 1 goto :fail

if exist "%POST_SCRIPT%" (
    echo Running %POST_SCRIPT%...
    call "%POST_SCRIPT%"
)

popd
endlocal
exit /b 0

:fail
popd
endlocal
exit /b 1
