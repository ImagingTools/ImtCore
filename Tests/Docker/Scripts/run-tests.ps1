# Run tests using Windows containers

param(
    [string]$ImageName = "imtcore-tests:windows",
    [string]$BaseUrl = "",
    [string]$StartPostgreSQL = "true",
    [string]$PostgresDB = "",
    [string]$DatabaseURL = "",
    [string]$TestUsername = "",
    [string]$TestPassword = ""
)

$ContainerName = "myapp-tests-$(Get-Random)"

Write-Host "==========================================" -ForegroundColor Cyan
Write-Host "Running Windows containers" -ForegroundColor Cyan
Write-Host "==========================================" -ForegroundColor Cyan
Write-Host ""
Write-Host "[DEBUG] Script path : $PSCommandPath" -ForegroundColor Gray
Write-Host "[DEBUG] Current dir : $(Get-Location)" -ForegroundColor Gray
Write-Host "[DEBUG] Container   : $ContainerName" -ForegroundColor Gray
Write-Host "[DEBUG] Image       : $ImageName" -ForegroundColor Gray
Write-Host "[DEBUG] BASE_URL    : $BaseUrl" -ForegroundColor Gray
Write-Host ""

# Check if Docker is installed
try {
    docker --version | Out-Null
} catch {
    Write-Host "Docker not found in PATH" -ForegroundColor Red
    exit 1
}

# Check if image exists
try {
    docker image inspect $ImageName | Out-Null
} catch {
    Write-Host "Image not found: $ImageName" -ForegroundColor Red
    exit 1
}

Write-Host ""
Write-Host "[DEBUG] Host Startup *.ps1:" -ForegroundColor Gray
Get-ChildItem -Path "Tests\Startup\*.ps1" -ErrorAction SilentlyContinue | ForEach-Object { Write-Host $_.Name -ForegroundColor Gray }
Write-Host "[DEBUG] Host GUI folder:" -ForegroundColor Gray
Get-ChildItem -Path "Tests\GUI" -ErrorAction SilentlyContinue | ForEach-Object { Write-Host $_.Name -ForegroundColor Gray }
Write-Host "[DEBUG] Host API folder:" -ForegroundColor Gray
Get-ChildItem -Path "Tests\API" -ErrorAction SilentlyContinue | ForEach-Object { Write-Host $_.Name -ForegroundColor Gray }
Write-Host ""

Write-Host "Starting idle container (entrypoint will NOT run yet)..." -ForegroundColor Yellow
docker run -d `
  --name $ContainerName `
  --entrypoint powershell `
  -e BASE_URL="$BaseUrl" `
  -e START_POSTGRESQL="$StartPostgreSQL" `
  -e POSTGRES_DB="$PostgresDB" `
  -e DATABASE_URL="$DatabaseURL" `
  -e TEST_USERNAME="$TestUsername" `
  -e TEST_PASSWORD="$TestPassword" `
  -e CI=true `
  $ImageName `
  -Command "Start-Sleep 3600"

if ($LASTEXITCODE -ne 0) {
    Write-Host "Failed to start container" -ForegroundColor Red
    exit 1
}

docker ps -a --filter "name=$ContainerName"

# Ensure target dirs exist
docker exec $ContainerName powershell -Command "New-Item -ItemType Directory -Force -Path C:\app\tests\GUI, C:\app\tests\API, C:\app\startup, C:\app\resources | Out-Null"

Write-Host ""
Write-Host "Copying GUI tests..." -ForegroundColor Yellow
if (Test-Path "Tests\GUI") {
    docker cp "Tests\GUI\." "${ContainerName}:C:\app\tests\GUI\"
}

Write-Host "Copying API tests..." -ForegroundColor Yellow
if (Test-Path "Tests\API") {
    docker cp "Tests\API\." "${ContainerName}:C:\app\tests\API\"
}

Write-Host "Copying resources..." -ForegroundColor Yellow
if (Test-Path "Tests\Resources") {
    docker cp "Tests\Resources\." "${ContainerName}:C:\app\resources\"
}

Write-Host "Copying startup scripts..." -ForegroundColor Yellow
if (Test-Path "Tests\Startup") {
    docker cp "Tests\Startup\." "${ContainerName}:C:\app\startup\"
}

Write-Host ""
Write-Host "[DEBUG] Container files after copy:" -ForegroundColor Gray
docker exec $ContainerName powershell -Command "Write-Host '--- C:\app\startup ---'; Get-ChildItem C:\app\startup -ErrorAction SilentlyContinue; Write-Host '--- C:\app\tests\GUI ---'; Get-ChildItem C:\app\tests\GUI -ErrorAction SilentlyContinue; Write-Host '--- C:\app\tests\API ---'; Get-ChildItem C:\app\tests\API -ErrorAction SilentlyContinue"

Write-Host ""
Write-Host "Running tests (entrypoint)..." -ForegroundColor Yellow
docker exec $ContainerName powershell -Command "`$env:PAUSE_BEFORE_TESTS='false'; C:\app\entrypoint.ps1"
$ExitCode = $LASTEXITCODE
Write-Host "[DEBUG] Test run exit code: $ExitCode" -ForegroundColor Gray

$ResultsDir = Join-Path (Split-Path $PSCommandPath) "test-results"

Write-Host "Copying test results..." -ForegroundColor Yellow
if (Test-Path $ResultsDir) {
    Remove-Item -Path $ResultsDir -Recurse -Force -ErrorAction SilentlyContinue
}
docker cp "${ContainerName}:C:\app\tests\test-results" $ResultsDir 2>$null
if ($LASTEXITCODE -ne 0) {
    Write-Host "No test results to copy" -ForegroundColor Gray
}

Write-Host "==========================================" -ForegroundColor Cyan
Write-Host "Container logs:" -ForegroundColor Cyan
Write-Host "==========================================" -ForegroundColor Cyan
docker logs $ContainerName

Write-Host "Cleaning up..." -ForegroundColor Yellow
docker rm -f $ContainerName | Out-Null

Write-Host "==========================================" -ForegroundColor Cyan
if ($ExitCode -eq 0) {
    Write-Host "Tests passed successfully" -ForegroundColor Green
} else {
    Write-Host "Tests failed with exit code: $ExitCode" -ForegroundColor Red
}
Write-Host "==========================================" -ForegroundColor Cyan

exit $ExitCode
