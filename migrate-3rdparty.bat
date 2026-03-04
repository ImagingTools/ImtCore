@echo off
setlocal enabledelayedexpansion

:: Migration script for converting 3rdParty libraries to separate git repositories
:: This script helps prepare each library for pushing to its own GitHub repository

:: Configuration
set "GITHUB_ORG=ImagingTools"
set "THIRD_PARTY_DIR=3rdParty"
set "TEMP_DIR=%TEMP%\3rdparty-migration"

:: List of libraries to migrate
set "LIBRARIES=EJ JQ JQML JQML2 QXlsx WebCompiler boost icu libmdbx mongoc mongocxx nodejs openssl pybind11 quazip zlib zstd"

:: Function to print colored output
goto :main

:print_info
    echo [INFO] %~1
    goto :eof

:print_warning
    echo [WARN] %~1
    goto :eof

:print_error
    echo [ERROR] %~1
    goto :eof

:prepare_library_repo
    set "lib_name=%~1"
    set "lib_path=%THIRD_PARTY_DIR%\%lib_name%"
    set "temp_repo=%TEMP_DIR%\%lib_name%"
    set "repo_name=%lib_name%"
    set "repo_url=https://github.com/%GITHUB_ORG%/%repo_name%.git"

    call :print_info "Preparing %lib_name%..."

    :: Check if library exists
    if not exist "%lib_path%" (
        call :print_error "Library not found: %lib_path%"
        exit /b 1
    )

    :: Create temporary directory for the repository
    if not exist "%temp_repo%" mkdir "%temp_repo%"
    
    :: Copy library contents
    call :print_info "Copying %lib_name% to temporary location..."
    xcopy /E /I /Q /Y "%lib_path%" "%temp_repo%" >nul
    
    :: Initialize git repository
    pushd "%temp_repo%"
    
    if not exist ".git" (
        call :print_info "Initializing git repository..."
        git init
        
        :: Set up Git LFS for large binary files
        call :print_info "Configuring Git LFS..."
        git lfs install
        
        :: Track large binary file types with LFS
        git lfs track "*.dll"
        git lfs track "*.lib"
        git lfs track "*.pyd"
        git lfs track "*.so"
        git lfs track "*.dylib"
        git lfs track "*.exe"
        git lfs track "*.a"
        
        :: Create README if it doesn't exist
        if not exist "README.md" (
            call :create_readme "%lib_name%" > README.md
        )
        
        :: Add .gitattributes first (created by git lfs track)
        if exist ".gitattributes" (
            git add .gitattributes
        )
        
        :: Add all files (LFS files will be handled automatically)
        git add .
        
        :: Create initial commit with multi-line message
        echo Initial commit of !lib_name! library from ImtCore/3rdParty > commit_msg.txt
        echo. >> commit_msg.txt
        echo This library was extracted from the ImtCore repository to enable >> commit_msg.txt
        echo independent version control and easier dependency management. >> commit_msg.txt
        echo. >> commit_msg.txt
        echo Configured with Git LFS for large binary files. >> commit_msg.txt
        echo. >> commit_msg.txt
        echo Source: ImagingTools/ImtCore/3rdParty/!lib_name! >> commit_msg.txt
        git commit -F commit_msg.txt
        del commit_msg.txt
        
        call :print_info "Repository prepared at: !temp_repo!"
        call :print_info "To push to GitHub, run:"
        echo   cd !temp_repo!
        echo   git remote add origin !repo_url!
        echo   git branch -M main
        echo   git push -u origin main
        echo.
    ) else (
        call :print_warning "Git repository already exists in %temp_repo%"
    )
    
    popd
    goto :eof

:create_readme
    set "lib_name=%~1"
    echo # %lib_name%
    echo.
    echo This repository contains the **%lib_name%** library used by ImtCore.
    echo.
    echo ## Origin
    echo.
    echo This library was extracted from the ImtCore repository (^`3rdParty/%lib_name%^`) 
    echo to enable independent version control and easier dependency management.
    echo.
    echo **Note**: This repository uses **Git LFS** (Large File Storage) for binary files (*.dll, *.lib, *.so, *.exe, etc.). Make sure you have Git LFS installed before cloning.
    echo.
    echo ## Usage
    echo.
    echo This repository is integrated into ImtCore as a git submodule:
    echo.
    echo ^`^`^`bash
    echo git submodule add https://github.com/%GITHUB_ORG%/%lib_name%.git 3rdParty/%lib_name%
    echo ^`^`^`
    echo.
    echo ## Cloning
    echo.
    echo To clone this repository with LFS files:
    echo.
    echo ^`^`^`bash
    echo git lfs install  # If not already installed
    echo git clone https://github.com/%GITHUB_ORG%/%lib_name%.git
    echo ^`^`^`
    echo.
    echo ## Updating
    echo.
    echo To update this library in ImtCore:
    echo.
    echo 1. Make changes in this repository
    echo 2. Commit and push changes
    echo 3. In ImtCore repository:
    echo    ^`^`^`bash
    echo    cd 3rdParty/%lib_name%
    echo    git pull origin main
    echo    cd ..\..
    echo    git add 3rdParty/%lib_name%
    echo    git commit -m "Update %lib_name% submodule"
    echo    git push
    echo    ^`^`^`
    echo.
    echo ## License
    echo.
    echo See LICENSE file if present, or refer to the original library's licensing terms.
    goto :eof

:prepare_all_libraries
    call :print_info "Starting migration of libraries..."
    
    :: Create temp directory
    if not exist "%TEMP_DIR%" mkdir "%TEMP_DIR%"
    
    for %%L in (%LIBRARIES%) do (
        call :prepare_library_repo "%%L"
    )
    
    call :print_info "All libraries prepared!"
    call :print_info "Temporary repositories created in: %TEMP_DIR%"
    echo.
    call :print_warning "Next steps:"
    echo 1. Create GitHub repositories for each library under %GITHUB_ORG%
    echo 2. Push each temporary repository to its corresponding GitHub repository
    echo 3. Run the add-submodules script to integrate them back into ImtCore
    goto :eof

:generate_add_submodules_script
    set "script_file=add-submodules.bat"
    
    call :print_info "Generating submodule addition script..."
    
    (
        echo @echo off
        echo setlocal enabledelayedexpansion
        echo.
        echo :: Script to add all 3rdParty libraries as git submodules
        echo :: Run this after creating and populating all GitHub repositories
        echo.
        echo set "GITHUB_ORG=ImagingTools"
        echo.
        echo :: Libraries to add as submodules
        echo set "LIBRARIES=EJ JQ JQML JQML2 QXlsx WebCompiler boost icu libmdbx mongoc mongocxx nodejs openssl pybind11 quazip zlib zstd"
        echo.
        echo echo Adding submodules...
        echo.
        echo for %%%%L in ^(%%LIBRARIES%%^) do ^(
        echo     set "repo_url=https://github.com/%%GITHUB_ORG%%/%%%%L.git"
        echo     set "submodule_path=3rdParty/%%%%L"
        echo     
        echo     echo Adding %%%%L...
        echo     git submodule add "^^!repo_url^^!" "^^!submodule_path^^!"
        echo ^)
        echo.
        echo echo Initializing and updating submodules...
        echo git submodule init
        echo git submodule update
        echo.
        echo echo Done! All submodules added.
        echo echo Don't forget to commit the changes:
        echo echo   git add .
        echo echo   git commit -m "Convert 3rdParty libraries to git submodules"
        echo.
        echo endlocal
    ) > "%script_file%"
    
    call :print_info "Created %script_file%"
    goto :eof

:show_usage
    echo Usage: %~nx0 [COMMAND]
    echo.
    echo Commands:
    echo     prepare-all     Prepare all libraries (create temp git repos)
    echo     prepare LIB     Prepare a specific library
    echo     generate-script Generate the add-submodules.bat script
    echo     list            List all libraries to be migrated
    echo     help            Show this help message
    echo.
    echo Examples:
    echo     %~nx0 prepare-all              # Prepare all libraries
    echo     %~nx0 prepare boost            # Prepare only boost library
    echo     %~nx0 generate-script          # Generate add-submodules.bat script
    echo     %~nx0 list                     # List all libraries
    echo.
    goto :eof

:main
    set "command=%~1"
    
    if "%command%"=="prepare-all" (
        call :prepare_all_libraries
        call :generate_add_submodules_script
        goto :end
    )
    
    if "%command%"=="prepare" (
        if "%~2"=="" (
            call :print_error "Please specify a library name"
            call :show_usage
            exit /b 1
        )
        call :prepare_library_repo "%~2"
        goto :end
    )
    
    if "%command%"=="generate-script" (
        call :generate_add_submodules_script
        goto :end
    )
    
    if "%command%"=="list" (
        call :print_info "Libraries to migrate:"
        for %%L in (%LIBRARIES%) do (
            echo   - %%L
        )
        goto :end
    )
    
    if "%command%"=="help" goto :usage
    if "%command%"=="--help" goto :usage
    if "%command%"=="-h" goto :usage
    if "%command%"=="/?" goto :usage
    
    if not "%command%"=="" (
        call :print_error "Unknown command: %command%"
    )
    
:usage
    call :show_usage
    if not "%command%"=="" exit /b 1

:end
    endlocal
