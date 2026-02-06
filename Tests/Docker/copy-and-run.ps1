# This script runs inside the Docker container
# It copies test files from mounted volumes and then runs the entrypoint

Write-Host "==========================================" -ForegroundColor Cyan
Write-Host "Docker Test Runner - File Copy & Execute" -ForegroundColor Cyan
Write-Host "==========================================" -ForegroundColor Cyan

# Copy GUI tests if mounted
if (Test-Path "C:\mnt\app\GUI") {
    Write-Host "Copying GUI tests from C:\mnt\app\GUI to C:\app\tests\GUI..." -ForegroundColor Yellow
    New-Item -ItemType Directory -Path "C:\app\tests\GUI" -Force | Out-Null
    Copy-Item -Path "C:\mnt\app\GUI\*" -Destination "C:\app\tests\GUI\" -Recurse -Force -ErrorAction SilentlyContinue
    Write-Host "✓ GUI tests copied" -ForegroundColor Green
}

# Copy API tests if mounted  
if (Test-Path "C:\mnt\app\API") {
    Write-Host "Copying API tests from C:\mnt\app\API to C:\app\tests\API..." -ForegroundColor Yellow
    New-Item -ItemType Directory -Path "C:\app\tests\API" -Force | Out-Null
    Copy-Item -Path "C:\mnt\app\API\*" -Destination "C:\app\tests\API\" -Recurse -Force -ErrorAction SilentlyContinue
    Write-Host "✓ API tests copied" -ForegroundColor Green
}

# Copy resources if mounted
if (Test-Path "C:\mnt\app\Resources") {
    Write-Host "Copying resources from C:\mnt\app\Resources to C:\app\resources..." -ForegroundColor Yellow
    New-Item -ItemType Directory -Path "C:\app\resources" -Force | Out-Null
    Copy-Item -Path "C:\mnt\app\Resources\*" -Destination "C:\app\resources\" -Recurse -Force -ErrorAction SilentlyContinue
    Write-Host "✓ Resources copied" -ForegroundColor Green
}

# Copy startup scripts if mounted
if (Test-Path "C:\mnt\app\Startup") {
    Write-Host "Copying startup scripts from C:\mnt\app\Startup to C:\app\startup..." -ForegroundColor Yellow
    New-Item -ItemType Directory -Path "C:\app\startup" -Force | Out-Null
    Copy-Item -Path "C:\mnt\app\Startup\*" -Destination "C:\app\startup\" -Recurse -Force -ErrorAction SilentlyContinue
    Write-Host "✓ Startup scripts copied" -ForegroundColor Green
}

# Install additional dependencies if package.json exists
if (Test-Path "C:\mnt\app\package.json") {
    Write-Host "Installing additional dependencies from C:\mnt\app\package.json..." -ForegroundColor Yellow
    Copy-Item -Path "C:\mnt\app\package.json" -Destination "C:\app\tests\" -Force
    Set-Location "C:\app\tests"
    npm install
    Write-Host "✓ Dependencies installed" -ForegroundColor Green
}

Write-Host ""
Write-Host "All files copied. Running entrypoint..." -ForegroundColor Cyan
Write-Host "==========================================" -ForegroundColor Cyan
Write-Host ""

# Run the entrypoint
$env:PAUSE_BEFORE_TESTS = "false"
& "C:\app\entrypoint.ps1"
exit $LASTEXITCODE
