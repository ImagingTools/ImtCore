# Example script for running tests using ImtCore Docker container (Windows)
# This script is meant to be used from application repositories (e.g., Lisa)
# 
# Usage:
#   1. Build ImtCore Docker image first (in ImtCore repository)
#   2. Run this script from your application repository
#   3. Tests will be executed automatically in the container

param(
    [string]$ImageName = "imtcore-tests:windows",
    [string]$BaseUrl = "http://host.docker.internal:3000",
    [string]$StartPostgreSQL = "false",
    [string]$PostgresDB = "",
    [string]$DatabaseURL = "",
    [string]$TestUsername = "",
    [string]$TestPassword = ""
)

$ErrorActionPreference = "Stop"

# Configuration
$ContainerName = "myapp-tests-$(Get-Date -Format 'yyyyMMddHHmmss')"

Write-Host "==========================================" -ForegroundColor Cyan
Write-Host "Running tests in ImtCore Docker container" -ForegroundColor Cyan
Write-Host "==========================================" -ForegroundColor Cyan

# Start the container in detached mode
Write-Host "Starting container: $ContainerName" -ForegroundColor Yellow
docker run -d `
  --name $ContainerName `
  -e BASE_URL="$BaseUrl" `
  -e START_POSTGRESQL="$StartPostgreSQL" `
  -e POSTGRES_DB="$PostgresDB" `
  -e DATABASE_URL="$DatabaseURL" `
  -e TEST_USERNAME="$TestUsername" `
  -e TEST_PASSWORD="$TestPassword" `
  -e CI="true" `
  $ImageName `
  powershell -Command "Start-Sleep -Seconds 3600"

if ($LASTEXITCODE -ne 0) {
    Write-Host "Failed to start container" -ForegroundColor Red
    exit 1
}

# Wait a bit for container to be ready
Start-Sleep -Seconds 2

# Copy GUI tests (Playwright)
if (Test-Path "Tests\GUI") {
    Write-Host "Copying GUI tests..." -ForegroundColor Yellow
    docker cp Tests\GUI\. "${ContainerName}:C:\app\tests\GUI\"
}

# Copy API tests (Postman)
if (Test-Path "Tests\API") {
    Write-Host "Copying API tests..." -ForegroundColor Yellow
    docker cp Tests\API\. "${ContainerName}:C:\app\tests\API\"
}

# Copy resources (installers, SQL scripts, configs, etc.)
if (Test-Path "Tests\Docker\Resources") {
    Write-Host "Copying resources..." -ForegroundColor Yellow
    docker cp Tests\Docker\Resources\. "${ContainerName}:C:\app\custom-apps\resources\"
}

# Copy startup scripts
if (Test-Path "Tests\Docker\Startup") {
    Write-Host "Copying startup scripts..." -ForegroundColor Yellow
    docker cp Tests\Docker\Startup\. "${ContainerName}:C:\app\custom-apps\startup\"
}

# Copy playwright.config.js if exists
if (Test-Path "Tests\playwright.config.js") {
    Write-Host "Copying playwright.config.js..." -ForegroundColor Yellow
    docker cp Tests\playwright.config.js "${ContainerName}:C:\app\tests\"
}

# Copy package.json if exists (for additional dependencies)
if (Test-Path "Tests\package.json") {
    Write-Host "Copying package.json..." -ForegroundColor Yellow
    docker cp Tests\package.json "${ContainerName}:C:\app\tests\"
    Write-Host "Installing additional dependencies..." -ForegroundColor Yellow
    docker exec $ContainerName npm install
}

# Restart container to trigger automatic test detection and execution
Write-Host "Running tests..." -ForegroundColor Yellow
docker restart $ContainerName

# Wait for container to finish
docker wait $ContainerName

# Get exit code
$ExitCode = docker inspect $ContainerName --format='{{.State.ExitCode}}'

# Copy test results out
Write-Host "Copying test results..." -ForegroundColor Yellow
docker cp "${ContainerName}:C:\app\tests\test-results" .\test-results -ErrorAction SilentlyContinue
if (-not $?) {
    Write-Host "No test results to copy" -ForegroundColor Gray
}

# Show logs
Write-Host "==========================================" -ForegroundColor Cyan
Write-Host "Container logs:" -ForegroundColor Cyan
Write-Host "==========================================" -ForegroundColor Cyan
docker logs $ContainerName

# Cleanup
Write-Host "Cleaning up..." -ForegroundColor Yellow
docker stop $ContainerName -ErrorAction SilentlyContinue | Out-Null
docker rm $ContainerName -ErrorAction SilentlyContinue | Out-Null

Write-Host "==========================================" -ForegroundColor Cyan
if ($ExitCode -eq 0) {
    Write-Host "✓ Tests passed successfully!" -ForegroundColor Green
} else {
    Write-Host "✗ Tests failed with exit code: $ExitCode" -ForegroundColor Red
}
Write-Host "==========================================" -ForegroundColor Cyan

exit $ExitCode
