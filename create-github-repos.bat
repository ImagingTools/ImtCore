@echo off
setlocal enabledelayedexpansion

:: Script to create GitHub repositories for all 3rdParty libraries
:: Requires GitHub CLI (gh) to be installed and authenticated

:: Configuration
set "GITHUB_ORG=ImagingTools"
set "VISIBILITY=public"

:: List of libraries
set "LIBRARIES=EJ QXlsx WebCompiler boost icu libmdbx mongoc mongocxx nodejs openssl pybind11 quazip zlib zstd"

:: Library descriptions
set "DESC_EJ=Export library for ImtCore"
set "DESC_QXlsx=Excel file library for ImtCore"
set "DESC_WebCompiler=Python-based web compiler for ImtCore"
set "DESC_boost=Boost C++ libraries (v1.75.0) for ImtCore"
set "DESC_icu=International Components for Unicode (v72.1) for ImtCore"
set "DESC_libmdbx=Memory-mapped database library for ImtCore"
set "DESC_mongoc=MongoDB C driver (v1.23.2) for ImtCore"
set "DESC_mongocxx=MongoDB C++ driver (v3.7.0) for ImtCore"
set "DESC_nodejs=Node.js runtime and npm for ImtCore"
set "DESC_openssl=OpenSSL cryptography library (v1.1) for ImtCore"
set "DESC_pybind11=Python/C++ binding library for ImtCore"
set "DESC_quazip=Qt/C++ wrapper for ZIP/UNZIP for ImtCore"
set "DESC_zlib=Compression library (v1.2.13) for ImtCore"
set "DESC_zstd=Zstandard compression library (v1.5.4) for ImtCore"

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

:check_gh_installed
    where gh >nul 2>&1
    if errorlevel 1 (
        call :print_error "GitHub CLI (gh) is not installed"
        echo Please install it from: https://cli.github.com/
        exit /b 1
    )
    goto :eof

:check_gh_authenticated
    gh auth status >nul 2>&1
    if errorlevel 1 (
        call :print_error "GitHub CLI is not authenticated"
        echo Please run: gh auth login
        exit /b 1
    )
    goto :eof

:get_description
    set "lib=%~1"
    set "desc=!DESC_%lib%!"
    if "!desc!"=="" set "desc=Third-party library for ImtCore"
    goto :eof

:create_repository
    set "lib_name=%~1"
    set "repo_name=%lib_name%"
    call :get_description "%lib_name%"
    set "description=!desc!"
    
    call :print_info "Creating repository: %GITHUB_ORG%/%repo_name%"
    
    :: Check if repository already exists
    gh repo view "%GITHUB_ORG%/%repo_name%" >nul 2>&1
    if not errorlevel 1 (
        call :print_warning "Repository %GITHUB_ORG%/%repo_name% already exists, skipping"
        exit /b 2
    )
    
    :: Create repository
    gh repo create "%GITHUB_ORG%/%repo_name%" --%VISIBILITY% --description "%description%" --confirm
    if errorlevel 1 (
        call :print_error "Failed to create %GITHUB_ORG%/%repo_name%"
        exit /b 1
    )
    
    call :print_info "Successfully created %GITHUB_ORG%/%repo_name%"
    exit /b 0

:create_all_repositories
    call :print_info "Creating repositories in %GITHUB_ORG% organization..."
    echo.
    
    set success_count=0
    set fail_count=0
    set skip_count=0
    
    for %%L in (%LIBRARIES%) do (
        call :create_repository "%%L"
        set create_result=!errorlevel!
        
        if !create_result! equ 0 (
            set /a success_count+=1
        ) else if !create_result! equ 2 (
            set /a skip_count+=1
        ) else (
            set /a fail_count+=1
        )
        echo.
    )
    
    echo.
    call :print_info "Summary:"
    echo   Created: !success_count!
    echo   Skipped: !skip_count!
    echo   Failed: !fail_count!
    
    if !fail_count! gtr 0 (
        call :print_warning "Some repositories failed to create. Please check errors above."
        exit /b 1
    )
    
    call :print_info "All repositories created successfully!"
    exit /b 0

:dry_run
    call :print_info "The following repositories would be created in %GITHUB_ORG%:"
    echo.
    
    for %%L in (%LIBRARIES%) do (
        set "repo_name=%%L"
        call :get_description "%%L"
        echo   %GITHUB_ORG%/%%L
        echo     Description: !desc!
        echo     Visibility: %VISIBILITY%
        echo.
    )
    goto :eof

:show_usage
    echo Usage: %~nx0 [COMMAND]
    echo.
    echo Commands:
    echo     create      Create all GitHub repositories
    echo     dry-run     Show what would be created without creating
    echo     help        Show this help message
    echo.
    echo Examples:
    echo     %~nx0 dry-run      # Preview what will be created
    echo     %~nx0 create       # Create all repositories
    echo.
    echo Prerequisites:
    echo     - GitHub CLI (gh) must be installed
    echo     - You must be authenticated: gh auth login
    echo     - You must have permission to create repos in %GITHUB_ORG%
    echo.
    goto :eof

:main
    set "command=%~1"
    
    if "%command%"=="create" (
        call :check_gh_installed
        if errorlevel 1 exit /b 1
        call :check_gh_authenticated
        if errorlevel 1 exit /b 1
        call :create_all_repositories
        goto :end
    )
    
    if "%command%"=="dry-run" (
        call :dry_run
        goto :end
    )
    
    if "%command%"=="help" goto :usage
    if "%command%"=="--help" goto :usage
    if "%command%"=="-h" goto :usage
    if "%command%"=="/?" goto :usage
    
    if not "%command%"=="" (
        call :print_error "Unknown command: %command%"
        echo.
    )
    
:usage
    call :show_usage
    if not "%command%"=="" exit /b 1

:end
    endlocal
