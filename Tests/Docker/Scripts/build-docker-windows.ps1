# Build ImtCore Docker image for Windows containers
# This script builds the Windows Docker image

param(
    [string]$ImageName = "imtcore-tests:windows"
)

Write-Host "==========================================" -ForegroundColor Cyan
Write-Host "Building ImtCore Docker Test Image" -ForegroundColor Cyan
Write-Host "(Windows)" -ForegroundColor Cyan
Write-Host "==========================================" -ForegroundColor Cyan
Write-Host ""

# Configuration
$Dockerfile = "Tests\Docker\Dockerfile.windows"

# Check if Docker is installed
try {
    docker --version | Out-Null
} catch {
    Write-Host "Error: Docker is not installed or not in PATH" -ForegroundColor Red
    exit 1
}

# Check if Dockerfile exists
if (-not (Test-Path $Dockerfile)) {
    Write-Host "Error: Dockerfile not found at $Dockerfile" -ForegroundColor Red
    Write-Host "Make sure you're running this script from the ImtCore repository root" -ForegroundColor Red
    exit 1
}

Write-Host "Building Windows image..." -ForegroundColor Yellow
Write-Host "Image: $ImageName" -ForegroundColor Yellow
Write-Host "Using Dockerfile: $Dockerfile" -ForegroundColor Yellow
Write-Host ""

# Build the Docker image
docker build -f $Dockerfile -t $ImageName .

if ($LASTEXITCODE -eq 0) {
    Write-Host ""
    Write-Host "==========================================" -ForegroundColor Cyan
    Write-Host "[SUCCESS] Build successful!" -ForegroundColor Green
    Write-Host "==========================================" -ForegroundColor Cyan
    Write-Host "Image: $ImageName" -ForegroundColor Yellow
    Write-Host ""
    Write-Host "To verify the image:" -ForegroundColor White
    Write-Host "  docker images | Select-String imtcore-tests" -ForegroundColor Gray
    Write-Host ""
    Write-Host "To use this image from an application repository:" -ForegroundColor White
    Write-Host "  1. Copy Tests\Docker\Scripts\run-tests.ps1 to your application repo" -ForegroundColor Gray
    Write-Host "  2. Run: .\run-tests.ps1" -ForegroundColor Gray
    Write-Host ""
} else {
    Write-Host ""
    Write-Host "==========================================" -ForegroundColor Cyan
    Write-Host "[FAILED] Build failed!" -ForegroundColor Red
    Write-Host "==========================================" -ForegroundColor Cyan
    exit 1
}
