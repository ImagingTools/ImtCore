# Simple configuration file for running Docker tests (Windows containers on Windows)
# Copy this file to your application's Tests directory and configure the variables below

# ==========================================
# CONFIGURATION
# ==========================================

# Application URL (use localhost for native Windows containers)
$env:BASE_URL = "http://localhost:8080"

# PostgreSQL password (optional, default: "root" if not set)
# Uncomment and set if you need a different password:
# $env:POSTGRES_PASSWORD = "your_password"

# Test credentials
$env:TEST_USERNAME = "test@example.com"
$env:TEST_PASSWORD = "testpassword"

# ==========================================
# DO NOT MODIFY BELOW THIS LINE
# ==========================================

# Determine ImtCore directory
if (-not $env:IMTCOREDIR) {
    # If IMTCOREDIR is not set, assume ImtCore is at the same level as the application
    $ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
    # Go up to application root (assuming script is in App\Tests\)
    $AppRoot = Split-Path -Parent $ScriptDir
    # Assume ImtCore is at same level as application
    $ImtCoreCandidate = Join-Path (Split-Path -Parent $AppRoot) "ImtCore"
    
    if (Test-Path (Join-Path $ImtCoreCandidate "Tests\Docker\Scripts\run-tests-windows-core.ps1")) {
        $env:IMTCOREDIR = $ImtCoreCandidate
    } else {
        Write-Host "ERROR: IMTCOREDIR environment variable is not set and ImtCore not found at expected location."
        Write-Host "Please either:"
        Write-Host '  1. Set IMTCOREDIR environment variable: $env:IMTCOREDIR = "C:\path\to\ImtCore"'
        Write-Host "  2. Place ImtCore at the same level as your application directory"
        exit 1
    }
}

# Validate ImtCore path
$CoreScriptPath = Join-Path $env:IMTCOREDIR "Tests\Docker\Scripts\run-tests-windows-core.ps1"
if (-not (Test-Path $CoreScriptPath)) {
    Write-Host "ERROR: Cannot find core script at: $CoreScriptPath"
    Write-Host "Please ensure IMTCOREDIR points to a valid ImtCore directory."
    exit 1
}

& $CoreScriptPath
exit $LASTEXITCODE
