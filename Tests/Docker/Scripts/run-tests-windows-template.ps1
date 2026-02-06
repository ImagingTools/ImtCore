# Simple configuration file for running Docker tests (Windows containers on Windows)
# Copy this file to your application's Tests directory and configure the variables below

# ==========================================
# CONFIGURATION
# ==========================================

# Application URL (use localhost for native Windows containers)
$env:BASE_URL = "http://localhost:8080"

# PostgreSQL database name (Note: PostgreSQL typically not available in Windows containers)
$env:POSTGRES_DB = ""

# PostgreSQL password (default: root)
$env:POSTGRES_PASSWORD = "root"

# Test credentials
$env:TEST_USERNAME = "test@example.com"
$env:TEST_PASSWORD = "testpassword"

# ==========================================
# DO NOT MODIFY BELOW THIS LINE
# ==========================================

# Check if IMTCOREDIR environment variable is set
if (-not $env:IMTCOREDIR) {
    Write-Host "ERROR: IMTCOREDIR environment variable is not set."
    Write-Host "Please set it to the path of your ImtCore directory:"
    Write-Host '  $env:IMTCOREDIR = "C:\path\to\ImtCore"'
    exit 1
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
