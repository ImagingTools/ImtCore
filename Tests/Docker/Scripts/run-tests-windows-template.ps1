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

# Auto-discover ImtCore (looks in parent directories)
$ScriptDir = Split-Path -Parent $PSCommandPath
$IMTCORE_PATH = Join-Path $ScriptDir "..\..\"

$CoreScriptPath = Join-Path $IMTCORE_PATH "Tests\Docker\Scripts\run-tests-windows-core.ps1"
if (-not (Test-Path $CoreScriptPath)) {
    $IMTCORE_PATH = Join-Path $ScriptDir "..\..\..\"
    $CoreScriptPath = Join-Path $IMTCORE_PATH "Tests\Docker\Scripts\run-tests-windows-core.ps1"
}

if (-not (Test-Path $CoreScriptPath)) {
    $IMTCORE_PATH = Join-Path $ScriptDir "..\..\..\..\"
    $CoreScriptPath = Join-Path $IMTCORE_PATH "Tests\Docker\Scripts\run-tests-windows-core.ps1"
}

if (-not (Test-Path $CoreScriptPath)) {
    Write-Host "ERROR: Cannot find ImtCore. Please ensure ImtCore is in a parent directory."
    exit 1
}

& $CoreScriptPath
exit $LASTEXITCODE
