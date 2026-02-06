# Template wrapper script for running tests in application repos
# Copy this file to your application repo's Tests directory and configure it for your needs

# ==========================================
# CONFIGURATION - MODIFY THESE FOR YOUR APP
# ==========================================

# Your application URL (use localhost for native Windows containers)
$env:BASE_URL = "http://localhost:8080"

# PostgreSQL settings (set START_POSTGRESQL=false if you don't need PostgreSQL)
# Note: PostgreSQL is typically not available in Windows containers
$env:START_POSTGRESQL = "false"
$env:POSTGRES_DB = ""
$env:DATABASE_URL = ""

# Test user credentials
$env:TEST_USERNAME = "test@example.com"
$env:TEST_PASSWORD = "testpassword"

# Docker image name (usually don't need to change this)
$env:IMAGE_NAME = "imtcore-tests:windows"

# ==========================================
# CORE LOGIC - DO NOT MODIFY BELOW THIS LINE
# ==========================================

# Find ImtCore path (adjust this path to point to your ImtCore location)
$IMTCORE_PATH = Join-Path (Split-Path $PSCommandPath) "..\..\..\..\ImtCore"

$CoreScriptPath = Join-Path $IMTCORE_PATH "Tests\Docker\Scripts\run-tests-windows-core.ps1"

if (-not (Test-Path $CoreScriptPath)) {
    Write-Host "ERROR: Cannot find ImtCore at $IMTCORE_PATH"
    Write-Host "Please update IMTCORE_PATH in this script to point to your ImtCore directory"
    exit 1
}

# Call the core script from ImtCore
& $CoreScriptPath
exit $LASTEXITCODE
