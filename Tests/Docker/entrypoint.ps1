# Docker entrypoint script for Windows test environment
# Handles startup sequence: PostgreSQL -> Custom Apps -> Tests

Write-Host "========================================" -ForegroundColor Green
Write-Host "Starting Test Environment (Windows)" -ForegroundColor Green
Write-Host "========================================" -ForegroundColor Green

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
        
        # Default password
        $POSTGRES_PASSWORD = if ($env:POSTGRES_PASSWORD) { $env:POSTGRES_PASSWORD } else { "root" }
        
        # Create log directory if needed
        if (-not (Test-Path $pgLog)) {
            New-Item -ItemType Directory -Path $pgLog -Force | Out-Null
        }
        
        # Ensure data dir exists
        if (-not (Test-Path $pgData)) {
            Write-Host "Initializing PostgreSQL data directory..." -ForegroundColor Yellow
            New-Item -ItemType Directory -Path $pgData -Force | Out-Null
            & "$pgBin\initdb.exe" -D $pgData -U postgres --auth-host=scram-sha-256
        }
        
        # Check for stale postmaster.pid
        $pidFile = Join-Path $pgData "postmaster.pid"
        if (Test-Path $pidFile) {
            $OLD_PID = Get-Content $pidFile -First 1
            if ($OLD_PID -and (Get-Process -Id $OLD_PID -ErrorAction SilentlyContinue)) {
                Write-Host "PostgreSQL appears to be running already (pid=$OLD_PID). Skipping start." -ForegroundColor Yellow
            } else {
                Write-Host "Found stale postmaster.pid (pid=$OLD_PID). Removing..." -ForegroundColor Yellow
                Remove-Item $pidFile -Force
                Write-Host "Starting PostgreSQL..." -ForegroundColor Yellow
                & "$pgBin\pg_ctl.exe" -D $pgData -l "$pgLog\logfile.log" start
            }
        } else {
            Write-Host "Starting PostgreSQL..." -ForegroundColor Yellow
            & "$pgBin\pg_ctl.exe" -D $pgData -l "$pgLog\logfile.log" start
        }
        
        # Wait for PostgreSQL
        $null = Wait-ForService "PostgreSQL" {
            & "$pgBin\psql.exe" -U postgres -c "SELECT 1" 2>$null
            return $?
        }
        
        # Set password
        Write-Host "Setting postgres password (default)" -ForegroundColor Yellow
        & "$pgBin\psql.exe" -U postgres -c "ALTER USER postgres WITH PASSWORD '$POSTGRES_PASSWORD';" 2>$null
        
        # Create test database if specified
        if ($env:POSTGRES_DB) {
            Write-Host "Creating database: $env:POSTGRES_DB" -ForegroundColor Yellow
            & "$pgBin\psql.exe" -U postgres -c "CREATE DATABASE $env:POSTGRES_DB" 2>$null
        }
        
        Write-Host "[OK] PostgreSQL initialized" -ForegroundColor Green
    } else {
        Write-Host "[WARNING] PostgreSQL not found in C:\Program Files\PostgreSQL" -ForegroundColor Yellow
        Write-Host "Continuing without PostgreSQL..." -ForegroundColor Yellow
    }
} else {
    Write-Host "Step 1: PostgreSQL startup skipped (START_POSTGRESQL not set to true)" -ForegroundColor Yellow
}

# Step 2: Run custom application installers and startup scripts
if (Test-Path "C:\app\startup") {
    Write-Host "Step 2: Running custom application scripts..." -ForegroundColor Yellow
    Write-Host "Startup dir: C:\app\startup" -ForegroundColor Yellow
    
    Write-Host "Listing C:\app\startup:" -ForegroundColor Yellow
    Get-ChildItem "C:\app\startup" -ErrorAction SilentlyContinue | ForEach-Object { Write-Host $_.Name }
    
    # Get all .ps1 scripts and sort them
    $startupScripts = Get-ChildItem "C:\app\startup\*.ps1" -ErrorAction SilentlyContinue | Sort-Object Name
    
    if ($startupScripts.Count -eq 0) {
        Write-Host "No startup scripts found: C:\app\startup\*.ps1" -ForegroundColor Yellow
    } else {
        Write-Host "Found $($startupScripts.Count) startup script(s):" -ForegroundColor Green
        foreach ($script in $startupScripts) {
            Write-Host "  - $($script.Name)" -ForegroundColor Green
        }
        
        Write-Host "Executing startup scripts in lexicographic order..." -ForegroundColor Yellow
        foreach ($script in $startupScripts) {
            Write-Host "Running startup script: $($script.Name)" -ForegroundColor Yellow
            & $script.FullName
            Write-Host "Finished: $($script.Name) (exit=$LASTEXITCODE)" -ForegroundColor Green
        }
    }
    
    Write-Host "[OK] Custom applications initialized" -ForegroundColor Green
} else {
    Write-Host "Step 2: No custom applications to start (C:\app\startup not found)" -ForegroundColor Yellow
}

# Step 3: Run tests or custom command
Write-Host "Step 3: Starting tests..." -ForegroundColor Yellow
Write-Host "========================================" -ForegroundColor Green

Set-Location C:\app\tests

if ($env:PAUSE_BEFORE_TESTS -eq "true") {
    Write-Host "PAUSE_BEFORE_TESTS=true -> pausing before running tests." -ForegroundColor Yellow
    Write-Host "Attach with: docker exec -it <container> powershell" -ForegroundColor Yellow
    Start-Sleep -Seconds 3600
}

# Auto-detect tests
Write-Host "Auto-detecting tests..." -ForegroundColor Yellow

$GUI_TESTS_FOUND = $false
$API_TESTS_FOUND = $false

# Check for GUI tests (Playwright)
if (Test-Path "C:\app\tests\GUI") {
    $guiTests = Get-ChildItem -Path "C:\app\tests\GUI" -Recurse -Include "*.spec.js", "*.spec.ts", "*.test.js", "*.test.ts" -ErrorAction SilentlyContinue
    if ($guiTests.Count -gt 0) {
        $GUI_TESTS_FOUND = $true
        Write-Host "[OK] Found Playwright tests in GUI folder" -ForegroundColor Green
    }
}

# Check for API tests (Postman/Newman)
if (Test-Path "C:\app\tests\API") {
    $apiTests = Get-ChildItem -Path "C:\app\tests\API" -Recurse -Filter "*collection*.json" -ErrorAction SilentlyContinue
    if ($apiTests.Count -gt 0) {
        $API_TESTS_FOUND = $true
        Write-Host "[OK] Found Postman collections in API folder" -ForegroundColor Green
    }
}

if ($GUI_TESTS_FOUND -or $API_TESTS_FOUND) {
    $EXIT_CODE = 0
    
    if ($GUI_TESTS_FOUND) {
        Write-Host "Preparing Playwright dependencies..." -ForegroundColor Yellow
        if (Test-Path "C:\app\tests\package.json") {
            if (Test-Path "C:\app\tests\package-lock.json") {
                Set-Location C:\app\tests
                npm ci
                if ($LASTEXITCODE -ne 0) { $EXIT_CODE = $LASTEXITCODE }
            } else {
                Set-Location C:\app\tests
                npm install
                if ($LASTEXITCODE -ne 0) { $EXIT_CODE = $LASTEXITCODE }
            }
        } else {
            Write-Host "No C:\app\tests\package.json found. Cannot install @playwright/test." -ForegroundColor Red
            $EXIT_CODE = 1
        }
        
        if ($EXIT_CODE -eq 0) {
            Write-Host "Running Playwright tests..." -ForegroundColor Yellow
            Set-Location C:\app\tests
            npx playwright test
            if ($LASTEXITCODE -ne 0) { $EXIT_CODE = $LASTEXITCODE }
        }
    }
    
    if ($API_TESTS_FOUND) {
        Write-Host "Running Postman tests..." -ForegroundColor Yellow
        
        # Use first environment file if exists (optional)
        $envFile = Get-ChildItem -Path "C:\app\tests\API" -Recurse -Filter "*environment*.json" -ErrorAction SilentlyContinue | Select-Object -First 1
        $envOpt = ""
        if ($envFile) {
            $envOpt = "-e `"$($envFile.FullName)`""
            Write-Host "Using environment: $($envFile.Name)" -ForegroundColor Yellow
        }
        
        $collections = Get-ChildItem -Path "C:\app\tests\API" -Recurse -Filter "*collection*.json" -ErrorAction SilentlyContinue
        foreach ($collection in $collections) {
            Write-Host "Running collection: $($collection.Name)" -ForegroundColor Yellow
            if ($envOpt) {
                newman run $collection.FullName $envOpt
            } else {
                newman run $collection.FullName
            }
            if ($LASTEXITCODE -ne 0) { $EXIT_CODE = $LASTEXITCODE }
        }
    }
    
    exit $EXIT_CODE
} else {
    Write-Host "No tests found in GUI or API folders" -ForegroundColor Yellow
    Write-Host "To run tests:" -ForegroundColor Yellow
    Write-Host "  - Copy Playwright tests to C:\app\tests\GUI\" -ForegroundColor Yellow
    Write-Host "  - Copy Postman collections to C:\app\tests\API\" -ForegroundColor Yellow
    exit 0
}
