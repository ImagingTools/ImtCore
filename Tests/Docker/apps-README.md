# Custom Applications Directory

This directory is for adding your own applications to the Docker test environment.

## Directory Structure

```
apps/
├── installers/          # Application installers (run once during startup)
│   ├── 01-install-puma.sh
│   └── 02-install-lisa.sh
├── startup/            # Startup scripts (run in order during container startup)
│   ├── 01-start-puma.sh
│   └── 02-start-lisa.sh
├── resources/          # Resource files (installers, executables, SQL files, etc.)
│   ├── app-installer.exe
│   ├── setup.sql
│   └── config.json
├── examples/           # Example scripts for reference
└── README.md           # This file
```

## Usage

### For Linux Containers

1. **Add Resource Files** (`resources/` directory):
   - Place your installer files (`.exe`, `.run`, `.deb`, `.rpm`, etc.)
   - Place SQL scripts, configuration files, or any other resources
   - These files will be accessible from your installer and startup scripts at `/app/custom-apps/resources/`

2. **Add Installer Scripts** (`installers/` directory):
   - Create shell scripts (`.sh`) that install your applications
   - Name them with prefixes like `01-`, `02-` to control execution order
   - Make them executable: `chmod +x installers/*.sh`
   - Reference resources using: `/app/custom-apps/resources/yourfile.ext`

3. **Add Startup Scripts** (`startup/` directory):
   - Create shell scripts (`.sh`) that start your applications
   - Use numbered prefixes to ensure correct startup order
   - Make them executable: `chmod +x startup/*.sh`
   - Reference resources using: `/app/custom-apps/resources/yourfile.ext`

### For Windows Containers

1. **Add Resource Files** (`resources/` directory):
   - Place your installer files (`.exe`, `.msi`, etc.)
   - Place SQL scripts, configuration files, or any other resources
   - These files will be accessible from your scripts at `C:\app\custom-apps\resources\`

2. **Add Installer Scripts** (`installers/` directory):
   - Create PowerShell scripts (`.ps1`) that install your applications
   - Name them with prefixes like `01-`, `02-` to control execution order
   - Reference resources using: `C:\app\custom-apps\resources\yourfile.ext`

3. **Add Startup Scripts** (`startup/` directory):
   - Create PowerShell scripts (`.ps1`) that start your applications
   - Use numbered prefixes to ensure correct startup order
   - Reference resources using: `C:\app\custom-apps\resources\yourfile.ext`

## Example: Lisa Application with Puma

### Linux Example

**installers/01-install-puma.sh:**
```bash
#!/bin/bash
set -e

echo "Installing Puma..."

# Method 1: Using installer from resources directory
if [ -f "/app/custom-apps/resources/puma-installer.run" ]; then
    echo "Installing from resources/puma-installer.run..."
    chmod +x /app/custom-apps/resources/puma-installer.run
    /app/custom-apps/resources/puma-installer.run --silent --install-dir=/opt/puma
    echo "Puma installed to /opt/puma"
fi

# Method 2: Using package/archive from resources
if [ -f "/app/custom-apps/resources/puma.tar.gz" ]; then
    echo "Extracting Puma from resources/puma.tar.gz..."
    mkdir -p /opt/puma
    tar -xzf /app/custom-apps/resources/puma.tar.gz -C /opt/puma
    echo "Puma extracted to /opt/puma"
fi

# Method 3: Using SQL script from resources for database setup
if [ -f "/app/custom-apps/resources/puma-setup.sql" ]; then
    echo "Running database setup from resources/puma-setup.sql..."
    psql -U postgres -f /app/custom-apps/resources/puma-setup.sql
    echo "Database setup completed"
fi

# Or install from package
# apt-get update && apt-get install -y puma

echo "Puma installed successfully"
```

**installers/02-install-lisa.sh:**
```bash
#!/bin/bash
set -e

echo "Installing Lisa..."

# Using installer from resources directory
if [ -f "/app/custom-apps/resources/lisa-installer.run" ]; then
    echo "Installing from resources/lisa-installer.run..."
    chmod +x /app/custom-apps/resources/lisa-installer.run
    /app/custom-apps/resources/lisa-installer.run --silent
    echo "Lisa installed successfully"
fi

# Apply database migrations from resources
if [ -f "/app/custom-apps/resources/lisa-migrations.sql" ]; then
    echo "Applying database migrations from resources..."
    psql -U postgres -d lisa_test -f /app/custom-apps/resources/lisa-migrations.sql
fi

echo "Lisa installation completed"
```

**startup/01-start-puma.sh:**
```bash
#!/bin/bash
set -e

echo "Starting Puma..."

# Start Puma in background
/usr/local/bin/puma &

# Wait for Puma to be ready
max_attempts=30
attempt=1
while [ $attempt -le $max_attempts ]; do
    if curl -f http://localhost:8080/health > /dev/null 2>&1; then
        echo "Puma is ready"
        exit 0
    fi
    echo "Waiting for Puma... ($attempt/$max_attempts)"
    sleep 2
    ((attempt++))
done

echo "Puma failed to start"
exit 1
```

**startup/02-start-lisa.sh:**
```bash
#!/bin/bash
set -e

echo "Starting Lisa..."

# Set environment variables for Lisa
export DATABASE_URL="postgresql://postgres@localhost:5432/lisa_test"
export PUMA_URL="http://localhost:8080"

# Start Lisa in background
/usr/local/bin/lisa-server &

# Wait for Lisa to be ready
max_attempts=30
attempt=1
while [ $attempt -le $max_attempts ]; do
    if curl -f http://localhost:3000/health > /dev/null 2>&1; then
        echo "Lisa is ready"
        exit 0
    fi
    echo "Waiting for Lisa... ($attempt/$max_attempts)"
    sleep 2
    ((attempt++))
done

echo "Lisa failed to start"
exit 1
```

### Windows Example

**installers/01-install-puma.ps1:**
```powershell
Write-Host "Installing Puma..." -ForegroundColor Yellow

# Using installer from resources directory
$installer = "C:\app\custom-apps\resources\puma-installer.exe"
if (Test-Path $installer) {
    Write-Host "Installing from C:\app\custom-apps\resources\puma-installer.exe..." -ForegroundColor Yellow
    & $installer /S /D="C:\Program Files\Puma"
    Write-Host "Puma installed successfully" -ForegroundColor Green
}

# Using SQL script from resources for database setup
$sqlScript = "C:\app\custom-apps\resources\puma-setup.sql"
if (Test-Path $sqlScript) {
    Write-Host "Running database setup from resources..." -ForegroundColor Yellow
    & "C:\Program Files\PostgreSQL\16\bin\psql.exe" -U postgres -f $sqlScript
    Write-Host "Database setup completed" -ForegroundColor Green
}

Write-Host "Puma installation completed" -ForegroundColor Green
```

**startup/01-start-puma.ps1:**
```powershell
Write-Host "Starting Puma..." -ForegroundColor Yellow

# Start Puma as background job
Start-Process "C:\Program Files\Puma\puma.exe" -NoNewWindow

# Wait for Puma to be ready
$maxAttempts = 30
for ($i = 1; $i -le $maxAttempts; $i++) {
    try {
        $response = Invoke-WebRequest -Uri "http://localhost:8080/health" -UseBasicParsing
        if ($response.StatusCode -eq 200) {
            Write-Host "Puma is ready" -ForegroundColor Green
            exit 0
        }
    } catch {
        # Not ready yet
    }
    Write-Host "Waiting for Puma... ($i/$maxAttempts)"
    Start-Sleep -Seconds 2
}

Write-Host "Puma failed to start" -ForegroundColor Red
exit 1
```

## Environment Variables

You can use these environment variables in your scripts:

- `START_POSTGRESQL` - Set to "true" to start PostgreSQL automatically
- `POSTGRES_DB` - Name of the database to create
- `BASE_URL` - Base URL for your application
- Add your own environment variables in docker-compose.yml

## Docker Compose Configuration

Update `docker-compose.linux.yml` or `docker-compose.windows.yml`:

```yaml
services:
  tests:
    build:
      context: ../..
      dockerfile: Tests/Docker/Dockerfile.linux
    environment:
      - START_POSTGRESQL=true
      - POSTGRES_DB=lisa_test
      - BASE_URL=http://localhost:3000
      - PUMA_URL=http://localhost:8080
    # ... rest of configuration
```

## Tips

1. **Executable Permissions**: Make sure Linux scripts are executable:
   ```bash
   chmod +x apps/installers/*.sh apps/startup/*.sh
   ```

2. **Resource Files**: Place all your installers, executables, and data files in the `resources/` directory
   - Windows: `.exe`, `.msi` installers
   - Linux: `.run`, `.deb`, `.rpm`, `.tar.gz` packages
   - SQL scripts: `.sql` files for database setup
   - Configuration files: `.json`, `.xml`, `.yaml`, etc.

3. **Dependencies**: Install scripts run before startup scripts, so install all dependencies first

4. **Startup Order**: Use numbered prefixes (01-, 02-, etc.) to control execution order

5. **Health Checks**: Always include health checks in startup scripts to ensure services are ready

6. **Logs**: Scripts output to console, which is captured in Docker logs

7. **Testing Locally**: Test your scripts locally before adding to Docker:
   ```bash
   cd Tests/Docker/apps
   ./installers/01-install-puma.sh
   ./startup/01-start-puma.sh
   ```

8. **Resource Path Reference**:
   - Linux: `/app/custom-apps/resources/yourfile.ext`
   - Windows: `C:\app\custom-apps\resources\yourfile.ext`

## Troubleshooting

- **Script not running**: Check if it's executable and has correct shebang (`#!/bin/bash`)
- **Service not starting**: Add more verbose logging to your startup scripts
- **PostgreSQL not available**: Set `START_POSTGRESQL=true` in docker-compose.yml
- **View logs**: `docker-compose logs tests`

## Examples Directory

Check `Tests/Docker/apps/examples/` for complete working examples.
