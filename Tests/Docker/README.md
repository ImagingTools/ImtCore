# ImtCore Docker Test Infrastructure

Complete Docker test infrastructure for running GUI (Playwright) and API (Postman) tests in isolated containers. Supports both Linux and Windows containers with automatic test detection.

## Quick Start

### 1. Build Docker Image (in ImtCore repository)

**Linux:**
```bash
./Tests/Docker/Scripts/build-docker-linux.sh
```

**Windows:**
```powershell
.\Tests\Docker\Scripts\build-docker-windows.bat
```

**Linux on Windows** (Docker Desktop in Linux mode):
```batch
.\Tests\Docker\Scripts\build-docker-linux-on-windows.bat
```
Or use Git Bash/WSL:
```bash
./Tests/Docker/Scripts/build-docker-linux-on-windows.sh
```

### 2. Run Tests (in your application repository)

Copy the run script to your application repository and execute:

**Linux:**
```bash
cp /path/to/ImtCore/Tests/Docker/Scripts/run-tests.sh .
chmod +x run-tests.sh
./run-tests.sh
```

**Windows:**
```powershell
Copy-Item "\path\to\ImtCore\Tests\Docker\Scripts\run-tests.bat" .
.\run-tests.bat
```

**Linux on Windows** (Docker Desktop in Linux mode):
```batch
copy C:\path\to\ImtCore\Tests\Docker\Scripts\run-tests-linux-on-windows.bat .
.\run-tests-linux-on-windows.bat
```
Or use Git Bash/WSL:
```bash
cp /path/to/ImtCore/Tests/Docker/Scripts/run-tests-linux-on-windows.sh .
chmod +x run-tests-linux-on-windows.sh
./run-tests-linux-on-windows.sh
```

## Features

- ✅ **Automatic Test Detection**: Scans GUI and API folders, runs tests automatically
- ✅ **Playwright**: Pre-installed with browsers and Verdana fonts for GUI testing
- ✅ **Newman (Postman CLI)**: Pre-installed for API testing
- ✅ **utils.js Helper**: Common Playwright utilities (login, screenshots, stability checks)
- ✅ **PostgreSQL Support**: Optional database with automatic setup
- ✅ **Cross-Platform**: Linux and Windows containers
- ✅ **Example Scripts**: Ready-to-use automation for building and running tests

## Directory Structure

```
Tests/Docker/
├── Scripts/                            # All build and run scripts
│   ├── build-docker-linux.sh           # Build script for Linux
│   ├── build-docker-windows.bat        # Build script for Windows  
│   ├── build-docker-linux-on-windows.bat  # Build Linux on Windows (native cmd)
│   ├── build-docker-linux-on-windows.sh   # Build Linux on Windows (Git Bash/WSL)
│   ├── run-tests.sh                    # Application test runner (Linux)
│   ├── run-tests.bat                   # Application test runner (Windows)
│   ├── run-tests-linux-on-windows.bat  # Test runner (Linux on Windows - native cmd)
│   └── run-tests-linux-on-windows.sh   # Test runner (Linux on Windows - Git Bash/WSL)
├── GUI/                                # GUI test utilities
│   └── utils.js                        # Playwright utilities helper
├── API/                                # API test utilities (placeholder)
├── Dockerfile.linux                    # Linux: Node 18 + Playwright + Newman + Fonts
├── Dockerfile.windows                  # Windows: Node 18 + Playwright + Newman
├── entrypoint.sh                       # Linux: startup + auto-detection
├── entrypoint.bat                      # Windows: startup + auto-detection
├── Resources/                          # For application installers, SQL, configs
└── Startup/                            # For application startup scripts

Tests/Fonts/
└── verdana/                            # Verdana fonts for consistent rendering
```

## Building Docker Images

### Linux (Recommended)

From ImtCore repository root:
```bash
./Tests/Docker/Scripts/build-docker-linux.sh
```

- Runs on Linux, macOS, Windows (via WSL 2)
- Fast build (~5 min), small image (~1.5 GB)
- CI/CD compatible

### Windows

From ImtCore repository root (PowerShell):
```powershell
.\Tests\Docker\build-docker-windows.bat
```

- Requires Windows 10/11 Pro+ with Hyper-V
- Slower build (~15 min), larger image (~5 GB)

### Linux on Windows

From ImtCore repository root:

**Command Prompt (Recommended):**
```batch
.\Tests\Docker\build-docker-linux-on-windows.bat
```

**Git Bash or WSL:**
```bash
./Tests/Docker/Scripts/build-docker-linux-on-windows.sh
```

- Checks Docker Desktop is in Linux mode
- Fast, small, CI/CD compatible
- **Recommended for Windows developers**

## Running Tests

### Using Example Scripts (Recommended)

The scripts handle everything automatically:

1. Start Docker container
2. Copy GUI tests, API tests, resources, startup scripts
3. Install dependencies
4. Run tests (auto-detected)
5. Collect results
6. Clean up

**Environment Variables:**
- `BASE_URL` - Application URL (Linux: http://localhost:3000, Windows: http://host.docker.internal:3000)
- `START_POSTGRESQL` - Start PostgreSQL (true/false)
- `POSTGRES_DB` - Database name
- `TEST_USERNAME`, `TEST_PASSWORD` - Test credentials
- `UPDATE_SNAPSHOTS` - Update Playwright reference screenshots (true/false, default: false)

**Updating Reference Screenshots:**

When you need to update Playwright visual regression baseline images:

```bash
# Linux
export UPDATE_SNAPSHOTS="true"
./run-tests.sh

# Windows
set UPDATE_SNAPSHOTS=true
.\run-tests.bat
```

This adds `--update-snapshots` flag to the Playwright command, which updates all screenshot baselines. After updating, commit the new screenshots to your repository.

**Linux Example:**
```bash
export BASE_URL="http://localhost:7776"
export START_POSTGRESQL="true"
export POSTGRES_DB="myapp_test"
./run-tests.sh
```

**Windows Example:**
```powershell
$env:BASE_URL="http://host.docker.internal:7776"
$env:START_POSTGRESQL="true"
$env:POSTGRES_DB="myapp_test"
.\run-tests.bat
```

**Linux on Windows Example:**
```batch
set BASE_URL=http://host.docker.internal:7776
set START_POSTGRESQL=true
set POSTGRES_DB=myapp_test
.\run-tests-linux-on-windows.bat
```
Or with Git Bash/WSL:
```bash
export BASE_URL="http://host.docker.internal:7776"
export START_POSTGRESQL="true"
export POSTGRES_DB="myapp_test"
./run-tests-linux-on-windows.sh
```

**Note:** When running Linux containers on Windows, use `host.docker.internal` to access services on the Windows host.

## Automatic Test Detection

**GUI Tests (Playwright):**
- Place in `Tests/GUI/` folder
- Files: `*.spec.js` or `*.spec.ts`
- Auto-runs: `npx playwright test GUI`

**API Tests (Postman):**
- Place in `Tests/API/` folder
- Files: `*.json` (Postman collections)
- Auto-runs: `newman run <collection>.json`

**Example:**
```
Tests/
├── GUI/
│   ├── login.spec.js          # ← Auto-detected
│   └── dashboard.spec.ts      # ← Auto-detected
└── API/
    ├── users.postman.json     # ← Auto-detected
    └── auth.postman.json      # ← Auto-detected
```

## Playwright Utilities (utils.js)

The container includes `/app/tests/GUI/utils.js` with common utilities:

**Functions:**
- `login(page, username, password, config, loginPaths)` - Automated login
- `clickOnElement(page, path, config)` - Click by objectName path
- `fillTextInput(page, text, path, config)` - Fill text input
- `selectComboBox(page, selectedText, path, config)` - Select dropdown
- `waitForPageStability(page, config)` - Wait for DOM stability
- `checkScreenshot(page, filename, masks, config)` - Screenshot comparison
- And more...

**Usage:**
```javascript
const { login, clickOnPage, checkScreenshot } = require('../GUI/utils.js');

test('user login', async ({ page }) => {
  await login(page, 'user', 'pass', defaultConfig, {
    username: ['LoginForm', 'UsernameInput'],
    password: ['LoginForm', 'PasswordInput'],
    submit: ['LoginForm', 'LoginButton']
  });
  
  await clickOnPage(page, 'Dashboard');
  await checkScreenshot(page, 'dashboard.png');
});
```

## Resources and Startup Scripts

### Resources

Place in `Tests/Docker/Resources/`:
- Installers: `.exe`, `.msi`, `.deb`, `.rpm`
- SQL scripts: database setup, migrations
- Configs: `.json`, `.yaml`, `.xml`
- Data files

### Startup Scripts

Place in `Tests/Docker/Startup/`:

**Naming** (execution order):
- `01-setup-database.sh` / `.bat`
- `02-install-app.sh` / `.bat`
- `03-start-services.sh` / `.bat`

**Environment Variables Available in Startup Scripts:**

Startup scripts can use these predefined environment variables:

| Variable | Linux Value | Windows Value |
|----------|-------------|---------------|
| `APP_DIR` | `/app` | `C:\app` |
| `STARTUP_DIR` | `/app/startup` | `C:\app\startup` |
| `RESOURCES_DIR` | `/app/resources` | `C:\app\resources` |
| `TESTS_DIR` | `/app/tests` | `C:\app\tests` |

**Example** (`01-setup-database.sh`):
```bash
#!/bin/bash
set -e
echo "Setting up database..."

# Use RESOURCES_DIR environment variable to access resources
psql -U postgres -f "$RESOURCES_DIR/schema.sql"
psql -U postgres -f "$RESOURCES_DIR/seed-data.sql"

echo "Database setup complete"
```

**Example** (`02-install-app.bat` for Windows):
```batch
@echo off
echo Installing application...

REM Use RESOURCES_DIR environment variable to access resources
copy "%RESOURCES_DIR%\config.json" "C:\MyApp\config.json"
"%RESOURCES_DIR%\installer.exe" /silent

echo Installation complete
```

**Example** (`01-start-app.sh`):
```bash
#!/bin/bash
set -e
echo "Starting MyApp..."

# Use RESOURCES_DIR for application files
/opt/myapp/bin/myapp-server --config "$RESOURCES_DIR/app-config.json" --port 3000 &

for i in {1..30}; do
    if curl -f http://localhost:3000/health; then
        echo "MyApp is ready"
        exit 0
    fi
    sleep 2
done
echo "Failed to start MyApp"
exit 1
```

## Troubleshooting

### Docker Not in Linux Mode (Windows)

**Solution:**
1. Right-click Docker Desktop tray icon
2. Select "Switch to Linux containers..."
3. Restart Docker

### Tests Not Detected

**Check:**
- Test files have correct extensions
- Files are in correct folders (`GUI/` or `API/`)
- View container logs: `docker logs <container-name>`

### Permission Denied (Linux)

```bash
chmod +x Tests/Docker/Scripts/build-docker-linux.sh
chmod +x Tests/Docker/Scripts/run-tests.sh
```

## Platform Comparison

| Feature | Linux | Windows | Linux-on-Windows |
|---------|-------|---------|------------------|
| Build Time | ~5 min | ~15 min | ~5 min |
| Image Size | ~1.5 GB | ~5 GB | ~1.5 GB |
| CI/CD | ✅ Yes | ⚠️ Limited | ✅ Yes |
| Recommended | ✅ General | Windows apps | ✅ Windows devs |

## CI/CD Integration

### GitHub Actions

```yaml
name: Tests
on: [push, pull_request]

jobs:
  test:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v3
      
      - name: Checkout ImtCore
        uses: actions/checkout@v3
        with:
          repository: ImagingTools/ImtCore
          path: ImtCore
      
      - name: Build Docker image
        run: cd ImtCore && ./Tests/Docker/Scripts/build-docker-linux.sh
      
      - name: Run tests
        run: ./run-tests.sh
```

## Summary

**For ImtCore:**
```bash
./Tests/Docker/Scripts/build-docker-linux.sh
```

**For Applications:**
1. Copy `run-tests.sh` from ImtCore
2. Create `Tests/GUI/` with Playwright tests
3. Create `Tests/API/` with Postman collections
4. Create `Tests/Docker/Resources/` with resources
5. Create `Tests/Docker/Startup/` with startup scripts
6. Run `./run-tests.sh`

Tests are auto-detected and executed. Results in `test-results/`.
