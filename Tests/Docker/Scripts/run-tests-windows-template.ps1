# Simple configuration file for running Docker tests (Windows containers on Windows)
# Copy this file to your application's Tests directory and configure the variables below

# ==========================================
# CONFIGURATION
# ==========================================

# Application URL (use localhost for native Windows containers)
$BASE_URL = "http://localhost:8080"

# PostgreSQL database name
$POSTGRES_DB = "myapp_test"

# PostgreSQL password (default: root)
$POSTGRES_PASSWORD = "root"

# Test credentials
$TEST_USERNAME = "test@example.com"
$TEST_PASSWORD = "testpassword"

# ==========================================
# DO NOT MODIFY BELOW THIS LINE
# ==========================================

# Get script directory (should be Tests\)
$ScriptDir = Split-Path -Parent $PSCommandPath

# Set container details
$IMAGE_NAME = "imtcore-tests:windows"
$CONTAINER_NAME = "myapp-tests-$(Get-Date -Format 'yyyyMMddHHmmss')"

# Auto-construct DATABASE_URL
$DATABASE_URL = "postgresql://postgres:$POSTGRES_PASSWORD@localhost:5432/$POSTGRES_DB"

Write-Host "==========================================" -ForegroundColor Cyan
Write-Host "Running Windows containers on Windows" -ForegroundColor Cyan
Write-Host "==========================================" -ForegroundColor Cyan
Write-Host "Container: $CONTAINER_NAME"
Write-Host "Image: $IMAGE_NAME"
Write-Host "BASE_URL: $BASE_URL"
Write-Host ""

# Check if Docker is installed
if (-not (Get-Command docker -ErrorAction SilentlyContinue)) {
    Write-Host "ERROR: Docker is not installed or not in PATH" -ForegroundColor Red
    exit 1
}

# Check if image exists
$imageExists = docker image inspect $IMAGE_NAME 2>$null
if ($LASTEXITCODE -ne 0) {
    Write-Host "ERROR: Docker image '$IMAGE_NAME' not found" -ForegroundColor Red
    Write-Host "Please build the image first using ImtCore build scripts"
    exit 1
}

# Run container with volume mounts
Write-Host "Starting container with test files..." -ForegroundColor Yellow
docker run --rm `
    --name $CONTAINER_NAME `
    -e BASE_URL=$BASE_URL `
    -e POSTGRES_DB=$POSTGRES_DB `
    -e POSTGRES_PASSWORD=$POSTGRES_PASSWORD `
    -e DATABASE_URL=$DATABASE_URL `
    -e TEST_USERNAME=$TEST_USERNAME `
    -e TEST_PASSWORD=$TEST_PASSWORD `
    -v "${ScriptDir}GUI:C:\mnt\app\GUI:ro" `
    -v "${ScriptDir}API:C:\mnt\app\API:ro" `
    -v "${ScriptDir}..\Resources:C:\mnt\app\Resources:ro" `
    -v "${ScriptDir}..\Startup:C:\mnt\app\Startup:ro" `
    -v "${ScriptDir}package.json:C:\mnt\app\package.json:ro" `
    $IMAGE_NAME `
    powershell -File C:\app\copy-and-run.ps1

$EXIT_CODE = $LASTEXITCODE

Write-Host ""
Write-Host "==========================================" -ForegroundColor Cyan
Write-Host "Tests completed with exit code: $EXIT_CODE" -ForegroundColor Cyan
Write-Host "==========================================" -ForegroundColor Cyan

exit $EXIT_CODE
