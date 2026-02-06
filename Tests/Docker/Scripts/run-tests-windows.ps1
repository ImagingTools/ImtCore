# Wrapper script - calls the core script with default/example configuration
# For production use in application repos, copy run-tests-windows-template.ps1 instead

# Set defaults if not already set
if (-not $env:IMAGE_NAME) { $env:IMAGE_NAME = "imtcore-tests:windows" }
if (-not $env:START_POSTGRESQL) { $env:START_POSTGRESQL = "false" }

# Call the core script from the same directory
$SCRIPT_DIR = Split-Path $PSCommandPath
& (Join-Path $SCRIPT_DIR "run-tests-windows-core.ps1")
exit $LASTEXITCODE
