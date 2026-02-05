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
& $Command[0] $Command[1..($Command.Length-1)]
