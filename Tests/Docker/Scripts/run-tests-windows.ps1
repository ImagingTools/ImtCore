# Wrapper script - calls the core script with default/example configuration
# For production use in application repos, copy run-tests-windows-template.ps1 instead

# Set defaults if not already set
if (-not $env:BASE_URL) { $env:BASE_URL = "http://localhost:8080" }
if (-not $env:POSTGRES_DB) { $env:POSTGRES_DB = "test_db" }
if (-not $env:POSTGRES_PASSWORD) { $env:POSTGRES_PASSWORD = "root" }

# Call the core script from the same directory
$SCRIPT_DIR = Split-Path $PSCommandPath
& (Join-Path $SCRIPT_DIR "run-tests-windows-core.ps1")
exit $LASTEXITCODE
