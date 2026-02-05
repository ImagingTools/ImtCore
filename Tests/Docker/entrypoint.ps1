# Docker entrypoint script for Windows test environment
# Handles startup sequence: PostgreSQL -> Custom Apps -> Tests

param(
    [string[]]$Command = @("npm.cmd", "test")
)

Write-Host "========================================"
Write-Host "Starting Test Environment (Windows)"
Write-Host "========================================"

# Function to wait for a service
function Wait-ForService {
    param(
        [string]$ServiceName,
        [scriptblock]$CheckCommand,
        [int]$MaxAttempts = 30
    )
    
    Write-Host "Waiting for $ServiceName to be ready..." -ForegroundColor Yellow
    
    for ($attempt = 1; $attempt -le $MaxAttempts; $attempt++) {
        try {
            $result = & $CheckCommand
            if ($result) {
                Write-Host "[OK] $ServiceName is ready" -ForegroundColor Green
                return $true
            }
        } catch {
            # Service not ready yet
        }
        Write-Host "  Attempt $attempt/$MaxAttempts..."
        Start-Sleep -Seconds 2
    }
    
    Write-Host "[ERROR] $ServiceName failed to start" -ForegroundColor Red
    return $false
}

# Step 1: Start PostgreSQL if needed
if ($env:START_POSTGRESQL -eq "true") {
    Write-Host "Step 1: Starting PostgreSQL..." -ForegroundColor Yellow
    
    # Find PostgreSQL installation
    $pgPath = Get-ChildItem "C:\Program Files\PostgreSQL" -Directory -ErrorAction SilentlyContinue | Select-Object -First 1
    if ($pgPath) {
        $pgBin = Join-Path $pgPath.FullName "bin"
        $pgData = "C:\PostgreSQL\data"
        $pgLog = "C:\PostgreSQL"
        
        # Create log directory if needed
        if (-not (Test-Path $pgLog)) {
            New-Item -ItemType Directory -Path $pgLog -Force | Out-Null
        }
        
        # Initialize database if needed
        if (-not (Test-Path $pgData)) {
            Write-Host "Initializing PostgreSQL data directory..."
            New-Item -ItemType Directory -Path $pgData -Force | Out-Null
            & "$pgBin\initdb.exe" -D $pgData -U postgres
        }
        
        # Start PostgreSQL
        & "$pgBin\pg_ctl.exe" -D $pgData -l "$pgLog\logfile.log" start
        
        # Wait for PostgreSQL
        Wait-ForService "PostgreSQL" {
            & "$pgBin\psql.exe" -U postgres -c "SELECT 1" 2>$null
            return $?
        }
        
        # Create test database if specified
        if ($env:POSTGRES_DB) {
            Write-Host "Creating database: $env:POSTGRES_DB" -ForegroundColor Yellow
            & "$pgBin\psql.exe" -U postgres -c "CREATE DATABASE $env:POSTGRES_DB" 2>$null
        }
    } else {
        Write-Host "[WARNING] PostgreSQL installation not found in C:\Program Files\PostgreSQL" -ForegroundColor Yellow
    }
} else {
    Write-Host "Step 1: PostgreSQL startup skipped (START_POSTGRESQL not set)" -ForegroundColor Yellow
}

# Step 2: Run custom application scripts
if (Test-Path "C:\app\custom-apps") {
    Write-Host "Step 2: Running custom application scripts..." -ForegroundColor Yellow
    
    # Run startup scripts in order
    if (Test-Path "C:\app\custom-apps\startup") {
        Get-ChildItem "C:\app\custom-apps\startup\*.ps1" | Sort-Object Name | ForEach-Object {
            Write-Host "Running startup script: $($_.Name)" -ForegroundColor Yellow
            & $_.FullName
        }
    }
    
    Write-Host "[OK] Custom applications initialized" -ForegroundColor Green
} else {
    Write-Host "Step 2: No custom applications to start" -ForegroundColor Yellow
}

# Step 3: Run tests
Write-Host "Step 3: Starting tests..." -ForegroundColor Yellow
Write-Host "========================================"

# Execute the main command
Set-Location C:\app\tests

# Check if we should run tests automatically based on folder contents
# If no command provided or default command, auto-detect tests
if ($Command.Count -eq 0 -or $Command[0] -eq "powershell") {
    Write-Host "Auto-detecting tests..." -ForegroundColor Yellow
    
    $GuiTestsFound = $false
    $ApiTestsFound = $false
    
    # Check for GUI tests (Playwright)
    if (Test-Path "C:\app\tests\GUI") {
        $guiFiles = Get-ChildItem -Path "C:\app\tests\GUI" -Filter "*.spec.js" -ErrorAction SilentlyContinue
        if (-not $guiFiles) {
            $guiFiles = Get-ChildItem -Path "C:\app\tests\GUI" -Filter "*.spec.ts" -ErrorAction SilentlyContinue
        }
        if ($guiFiles) {
            $GuiTestsFound = $true
            Write-Host "[OK] Found Playwright tests in GUI folder" -ForegroundColor Green
        }
    }
    
    # Check for API tests (Postman/Newman)
    if (Test-Path "C:\app\tests\API") {
        $apiFiles = Get-ChildItem -Path "C:\app\tests\API" -Filter "*.json" -ErrorAction SilentlyContinue
        if ($apiFiles) {
            $ApiTestsFound = $true
            Write-Host "[OK] Found Postman collections in API folder" -ForegroundColor Green
        }
    }
    
    # Run tests based on what was found
    if ($GuiTestsFound -or $ApiTestsFound) {
        $ExitCode = 0
        
        if ($GuiTestsFound) {
            Write-Host "Running Playwright tests..." -ForegroundColor Yellow
            & npx playwright test GUI
            if ($LASTEXITCODE -ne 0) { $ExitCode = $LASTEXITCODE }
        }
        
        if ($ApiTestsFound) {
            Write-Host "Running Postman tests..." -ForegroundColor Yellow
            Get-ChildItem -Path "C:\app\tests\API" -Filter "*.json" | ForEach-Object {
                Write-Host "Running collection: $($_.Name)" -ForegroundColor Yellow
                & newman run $_.FullName
                if ($LASTEXITCODE -ne 0) { $ExitCode = $LASTEXITCODE }
            }
        }
        
        exit $ExitCode
    } else {
        Write-Host "No tests found in GUI or API folders" -ForegroundColor Yellow
        Write-Host "To run tests:" -ForegroundColor Yellow
        Write-Host "  - Copy Playwright tests to C:\app\tests\GUI\" -ForegroundColor Yellow
        Write-Host "  - Copy Postman collections to C:\app\tests\API\" -ForegroundColor Yellow
        exit 0
    }
} else {
    # Run the provided command
    & $Command[0] $Command[1..($Command.Length-1)]
}
