# Core test runner script for Windows containers on Windows
# This script should NOT be modified by application repos
# Applications should use a wrapper script that sets environment variables and calls this script

$ErrorActionPreference = "Stop"

# Generate unique container name if not provided
if (-not $env:CONTAINER_NAME) {
    $env:CONTAINER_NAME = "imtcore-tests-$(Get-Date -Format 'yyyyMMddHHmmss')"
}

# Set constants and defaults
$env:IMAGE_NAME = "imtcore-tests:windows"
if (-not $env:POSTGRES_PASSWORD) { $env:POSTGRES_PASSWORD = "root" }
if (-not $env:POSTGRES_DB) { $env:POSTGRES_DB = "test_db" }

# Auto-construct DATABASE_URL (Note: PostgreSQL typically not available in Windows containers)
if ($env:POSTGRES_DB) {
    $env:DATABASE_URL = "postgresql://postgres:$($env:POSTGRES_PASSWORD)@localhost:5432/$($env:POSTGRES_DB)"
}

Write-Host "=========================================="
Write-Host "Running Windows containers on Windows"
Write-Host "=========================================="
Write-Host ""
Write-Host "[DEBUG] Script path: $PSCommandPath"
Write-Host "[DEBUG] Current dir: $(Get-Location)"
Write-Host "[DEBUG] Container: $env:CONTAINER_NAME"
Write-Host "[DEBUG] Image: $env:IMAGE_NAME"
Write-Host "[DEBUG] BASE_URL: $env:BASE_URL"
Write-Host "[DEBUG] POSTGRES_DB: $env:POSTGRES_DB"
Write-Host "[DEBUG] POSTGRES_PASSWORD: $env:POSTGRES_PASSWORD"
Write-Host "[DEBUG] DATABASE_URL: $env:DATABASE_URL"
Write-Host "[DEBUG] TEST_USERS: $env:TEST_USERS"
Write-Host ""

# Check if Docker is available
if (-not (Get-Command docker -ErrorAction SilentlyContinue)) {
    Write-Host "ERROR: Docker is not installed or not in PATH"
    exit 1
}

# Check if image exists
docker image inspect $env:IMAGE_NAME *> $null
if ($LASTEXITCODE -ne 0) {
    Write-Host "ERROR: Docker image '$env:IMAGE_NAME' not found"
    Write-Host "Please build the image first using build-docker-windows.ps1"
    exit 1
}

Write-Host "[DEBUG] Host Tests directory structure:"
if (Test-Path "Tests\Startup") {
    Write-Host "[DEBUG] Host Startup *.ps1:"
    Get-ChildItem "Tests\Startup\*.ps1" -ErrorAction SilentlyContinue | ForEach-Object { Write-Host "  $($_.Name)" }
    if (-not (Get-ChildItem "Tests\Startup\*.ps1" -ErrorAction SilentlyContinue)) {
        Write-Host "  (no .ps1 files)"
    }
}
if (Test-Path "Tests\GUI") {
    Write-Host "[DEBUG] Host GUI folder:"
    Get-ChildItem "Tests\GUI" -ErrorAction SilentlyContinue | ForEach-Object { Write-Host "  $($_.Name)" }
}
if (Test-Path "Tests\API") {
    Write-Host "[DEBUG] Host API folder:"
    Get-ChildItem "Tests\API" -ErrorAction SilentlyContinue | ForEach-Object { Write-Host "  $($_.Name)" }
}
Write-Host ""

Write-Host "Starting idle container (entrypoint will NOT run yet)..."
docker run -d `
  --name $env:CONTAINER_NAME `
  --entrypoint powershell `
  -e BASE_URL="$env:BASE_URL" `
  -e START_POSTGRESQL="false" `
  -e POSTGRES_PASSWORD="$env:POSTGRES_PASSWORD" `
  -e POSTGRES_DB="$env:POSTGRES_DB" `
  -e DATABASE_URL="$env:DATABASE_URL" `
  -e TEST_USERS="$env:TEST_USERS" `
  -e CI=true `
  $env:IMAGE_NAME `
  -Command "Start-Sleep -Seconds 3600"

if ($LASTEXITCODE -ne 0) {
    Write-Host "ERROR: Failed to start container"
    exit 1
}

docker ps -a --filter "name=$env:CONTAINER_NAME"

# Ensure target dirs exist
Write-Host ""
Write-Host "Creating directories in container..."
docker exec $env:CONTAINER_NAME powershell -Command "New-Item -ItemType Directory -Force -Path C:\app\tests\GUI,C:\app\tests\API,C:\app\startup,C:\app\resources | Out-Null"

Write-Host ""
Write-Host "Copying GUI tests..."
if (Test-Path "Tests\GUI") {
    docker cp "Tests\GUI\." "$($env:CONTAINER_NAME):C:\app\tests\GUI\"
} else {
    Write-Host "WARNING: Tests\GUI directory not found - skipping"
}

Write-Host "Copying API tests..."
if (Test-Path "Tests\API") {
    docker cp "Tests\API\." "$($env:CONTAINER_NAME):C:\app\tests\API\"
} else {
    Write-Host "WARNING: Tests\API directory not found - skipping"
}

Write-Host "Copying resources..."
if (Test-Path "Tests\Resources") {
    docker cp "Tests\Resources\." "$($env:CONTAINER_NAME):C:\app\resources\"
} else {
    Write-Host "WARNING: Tests\Resources directory not found - skipping"
}

Write-Host "Copying startup scripts..."
if (Test-Path "Tests\Startup") {
    docker cp "Tests\Startup\." "$($env:CONTAINER_NAME):C:\app\startup\"
} else {
    Write-Host "WARNING: Tests\Startup directory not found - skipping"
}

Write-Host ""
Write-Host "[DEBUG] Container files after copy:"
docker exec $env:CONTAINER_NAME powershell -Command @"
Write-Host '--- C:\app\startup ---'
Get-ChildItem C:\app\startup -ErrorAction SilentlyContinue | Format-Table -AutoSize
if (-not (Get-ChildItem C:\app\startup -ErrorAction SilentlyContinue)) { Write-Host '(empty)' }
Write-Host '--- C:\app\tests\GUI ---'
Get-ChildItem C:\app\tests\GUI -ErrorAction SilentlyContinue | Format-Table -AutoSize
if (-not (Get-ChildItem C:\app\tests\GUI -ErrorAction SilentlyContinue)) { Write-Host '(empty)' }
Write-Host '--- C:\app\tests\API ---'
Get-ChildItem C:\app\tests\API -ErrorAction SilentlyContinue | Format-Table -AutoSize
if (-not (Get-ChildItem C:\app\tests\API -ErrorAction SilentlyContinue)) { Write-Host '(empty)' }
"@

Write-Host ""
Write-Host "Running tests (entrypoint)..."
docker exec $env:CONTAINER_NAME powershell -Command "`$env:PAUSE_BEFORE_TESTS='false'; C:\app\entrypoint.ps1"
$EXIT_CODE = $LASTEXITCODE

Write-Host "[DEBUG] Test run exit code: $EXIT_CODE"

$RESULTS_DIR = Join-Path (Split-Path $PSCommandPath) "test-results"

Write-Host ""
Write-Host "Copying test results..."
if (Test-Path $RESULTS_DIR) {
    Remove-Item -Recurse -Force $RESULTS_DIR -ErrorAction SilentlyContinue
}
docker cp "$($env:CONTAINER_NAME):C:\app\tests\test-results" $RESULTS_DIR 2>$null
if ($LASTEXITCODE -ne 0) {
    Write-Host "No test results to copy"
}

Write-Host ""
Write-Host "=========================================="
Write-Host "Container logs:"
Write-Host "=========================================="
docker logs $env:CONTAINER_NAME

Write-Host ""
Write-Host "Cleaning up container..."
docker rm -f $env:CONTAINER_NAME *> $null

Write-Host "=========================================="
if ($EXIT_CODE -eq 0) {
    Write-Host "Tests passed successfully!"
} else {
    Write-Host "Tests failed with exit code: $EXIT_CODE"
}
Write-Host "=========================================="

exit $EXIT_CODE
