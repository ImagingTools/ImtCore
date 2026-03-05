@echo off
setlocal enabledelayedexpansion

:: Script to add all 3rdParty libraries as git submodules
:: Run this after creating and populating all GitHub repositories

set "GITHUB_ORG=ImagingTools"

:: Libraries to add as submodules
set "LIBRARIES=EJ JQ JQML JQML2 QXlsx WebCompiler boost icu libmdbx mongoc mongocxx nodejs openssl pybind11 quazip zlib zstd"

echo Adding submodules...

for %%L in (%LIBRARIES%) do (
    set "repo_url=https://github.com/%GITHUB_ORG%/%%L.git"
    set "submodule_path=3rdParty/%%L"
    
    echo Adding %%L...
    git submodule add "!repo_url!" "!submodule_path!"
)

echo Initializing and updating submodules...
git submodule init
git submodule update

echo Done All submodules added.
echo Don't forget to commit the changes:
echo   git add .
echo   git commit -m "Convert 3rdParty libraries to git submodules"

endlocal
