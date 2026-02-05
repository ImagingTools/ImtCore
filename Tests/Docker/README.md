# ImtCore Docker Test Environment

This directory contains the Docker infrastructure for running tests in containerized environments, supporting both **Linux** and **Windows** containers. This is the **base infrastructure** that should be used by application repositories (like Lisa) to run their tests.

## Key Features

- **Automatic Test Detection**: Container automatically detects and runs tests based on folder contents
- **Playwright Support**: Pre-installed for GUI/browser testing with Verdana fonts
- **Postman/Newman Support**: Pre-installed for API testing
- **Playwright Helper**: `utils.js` included with common test utilities (login, screenshots, stability checks)
- **GUI Tests**: Place Playwright tests in `GUI/` folder - automatically detected and run
- **API Tests**: Place Postman collections in `API/` folder - automatically detected and run
- **Example Scripts**: Ready-to-use scripts for Linux and Windows (`run-tests.sh`, `run-tests.ps1`)
- **Cross-Platform**: Supports both Linux and Windows containers

## Overview

The test execution flow is:

1. **Build Docker image** from ImtCore - creates the base test environment with Playwright and Newman
2. **Run container** from application repository (e.g., Lisa) 
3. **Copy files** from application repository into the running container:
   - GUI tests (Playwright `.spec.js` or `.spec.ts` files) → `/app/tests/GUI/`
   - API tests (Postman `.json` collections) → `/app/tests/API/`
   - Resources (installers, SQL scripts, config files) → `/app/custom-apps/resources/`
   - Startup scripts (application initialization) → `/app/custom-apps/startup/`
4. **Tests run automatically**: Container detects tests in GUI and API folders and runs them

## Directory Structure

```
Tests/Docker/
├── Dockerfile.linux          # Linux container configuration
├── Dockerfile.windows        # Windows container configuration
├── docker-compose.linux.yml  # Linux compose configuration
├── docker-compose.windows.yml# Windows compose configuration
├── entrypoint.sh             # Linux startup script
├── entrypoint.ps1            # Windows startup script
├── utils.js                  # Playwright test helper utilities
├── run-tests.sh              # Example script for Linux
├── run-tests.ps1             # Example script for Windows
├── .dockerignore             # Files to exclude from build
├── Resources/                # Base resources from ImtCore
│   ├── .gitkeep             # Keeps directory in git
│   └── README.md            # Resources documentation
├── Startup/                  # Base startup scripts from ImtCore
│   └── .gitkeep             # Keeps directory in git
└── README.md                # This file

Tests/Fonts/
└── verdana/                  # Verdana font files for Playwright
    ├── verdana.ttf
    ├── verdanab.ttf
    ├── verdanai.ttf
    └── verdanaz.ttf
```

## Container Structure

### Linux Container
When the Linux container is built, it creates the following structure:

```
/app/
├── tests/                   # Test execution directory
│   ├── GUI/                # GUI tests (copied from application repo)
│   ├── API/                # API tests (copied from application repo)
│   └── test-results/       # Test results output
└── custom-apps/            # Custom application files
    ├── resources/          # Installation resources
    │   └── (files from ImtCore + application repo)
    └── startup/            # Startup scripts
        └── (scripts from ImtCore + application repo)
```

### Windows Container
Windows container structure (paths use Windows notation):

```
C:\app\
├── tests\                   # Test execution directory
│   ├── GUI\                # GUI tests (copied from application repo)
│   ├── API\                # API tests (copied from application repo)
│   └── test-results\       # Test results output
└── custom-apps\            # Custom application files
    ├── resources\          # Installation resources
    │   └── (files from ImtCore + application repo)
    └── startup\            # Startup scripts
        └── (scripts from ImtCore + application repo)
```

## Building the Docker Image

### Linux Containers

From the ImtCore repository root:

```bash
# Build the Linux image
docker build -f Tests/Docker/Dockerfile.linux -t imtcore-tests:linux .

# Or using docker-compose
docker-compose -f Tests/Docker/docker-compose.linux.yml build
```

### Windows Containers

**Requirements:**
- Windows 10/11 Pro, Enterprise, or Windows Server
- Docker Desktop with Windows containers mode enabled
- Hyper-V feature enabled

From the ImtCore repository root (PowerShell or CMD):

```powershell
# Build the Windows image
docker build -f Tests/Docker/Dockerfile.windows -t imtcore-tests:windows .

# Or using docker-compose
docker-compose -f Tests/Docker/docker-compose.windows.yml build
```

**Note:** Windows containers can only be built on Windows. Building takes longer (~15-20 minutes first time) and produces larger images (~5 GB) compared to Linux containers.

### Running Linux Containers on Windows

You can run Linux containers on Windows using Docker Desktop's Linux mode (default):

1. Ensure Docker Desktop is in Linux containers mode
2. Build the Linux image as shown above
3. The container runs using WSL 2 backend

This is the **recommended approach** for most cases as Linux containers are:
- Faster to build (~5 minutes vs ~15 minutes)
- Smaller in size (~1.5 GB vs ~5 GB)
- Compatible with CI/CD (GitHub Actions, GitLab CI, etc.)
- Work on all platforms (Linux, macOS, Windows)

## Using from Application Repository (e.g., Lisa)

### Option 1: Use the Provided Example Scripts (Recommended)

ImtCore provides ready-to-use example scripts that you can copy to your application repository:

**Linux:**
1. Copy `Tests/Docker/run-tests.sh` from ImtCore to your application repository
2. Customize environment variables at the top of the script if needed
3. Run: `./run-tests.sh`

**Windows:**
1. Copy `Tests/Docker/run-tests.ps1` from ImtCore to your application repository
2. Customize parameters if needed
3. Run: `.\run-tests.ps1`

The example scripts automatically:
- Start the Docker container
- Copy GUI tests, API tests, resources, and startup scripts
- Install additional dependencies (if package.json exists)
- Run tests and collect results
- Clean up the container

### Option 2: Manual Setup

### Step 1: Build the Base Image

From your ImtCore repository:

**Linux:**
```bash
docker build -f Tests/Docker/Dockerfile.linux -t imtcore-tests:linux .
```

**Windows:**
```powershell
docker build -f Tests/Docker/Dockerfile.windows -t imtcore-tests:windows .
```

### Step 2: Create Run Script in Your Application Repository

**Linux script** (`run-tests-in-docker.sh`):

```bash
#!/bin/bash
set -e

# Configuration
CONTAINER_NAME="lisa-tests"
IMAGE_NAME="imtcore-tests:linux"

# Start the container in detached mode
docker run -d \
  --name "$CONTAINER_NAME" \
  --network host \
  -e BASE_URL="http://localhost:7776" \
  -e START_POSTGRESQL="true" \
  -e POSTGRES_DB="lisa_test" \
  -e DATABASE_URL="postgresql://postgres@localhost:5432/lisa_test" \
  -e TEST_USERNAME="su" \
  -e TEST_PASSWORD="1" \
  "$IMAGE_NAME" \
  sleep infinity

# Copy test files and resources
echo "Copying test files..."
docker cp Tests/GUI "$CONTAINER_NAME:/app/tests/"
docker cp Tests/API "$CONTAINER_NAME:/app/tests/"

# Copy resources (installers, SQL scripts, etc.)
echo "Copying resources..."
docker cp Tests/Docker/Resources/. "$CONTAINER_NAME:/app/custom-apps/resources/"

# Copy startup scripts
echo "Copying startup scripts..."
docker cp Tests/Docker/Startup/. "$CONTAINER_NAME:/app/custom-apps/startup/"
# Make scripts executable
docker exec "$CONTAINER_NAME" chmod +x /app/custom-apps/startup/*.sh

# Restart container to run startup scripts and auto-detect tests
echo "Running tests..."
docker restart "$CONTAINER_NAME"
docker wait "$CONTAINER_NAME"

# Copy test results out
echo "Copying test results..."
docker cp "$CONTAINER_NAME:/app/tests/test-results" ./

# Cleanup
docker stop "$CONTAINER_NAME" 2>/dev/null || true
docker rm "$CONTAINER_NAME"
```

## Automatic Test Detection

The container automatically detects and runs tests based on the presence of files in specific folders:

### GUI Tests (Playwright)
- **Location**: `/app/tests/GUI/` (Linux) or `C:\app\tests\GUI\` (Windows)
- **Detection**: Looks for `*.spec.js` or `*.spec.ts` files
- **Execution**: Runs `npx playwright test GUI` if tests are found

### API Tests (Postman/Newman)
- **Location**: `/app/tests/API/` (Linux) or `C:\app\tests\API\` (Windows)
- **Detection**: Looks for `*.json` files (Postman collections)
- **Execution**: Runs `newman run <collection>.json` for each collection found

### Example Test Structure

```
/app/tests/
├── GUI/
│   ├── login.spec.js         # Playwright test
│   ├── dashboard.spec.ts     # Playwright test
│   └── playwright.config.js  # Optional config
└── API/
    ├── user-api.json         # Postman collection
    └── auth-api.json         # Postman collection
```

When the container starts (or restarts), it will:
1. Run startup scripts from `/app/custom-apps/startup/`
2. Auto-detect tests in GUI and API folders
3. Run Playwright tests if GUI folder contains `.spec.js` or `.spec.ts` files
4. Run Newman/Postman tests if API folder contains `.json` files
5. Exit with appropriate status code

**Note**: If no tests are found, the container will exit successfully with a message.

## Playwright Test Utilities (utils.js)

The container includes a pre-installed `utils.js` helper file at `/app/tests/utils.js` (Linux) or `C:\app\tests\utils.js` (Windows) with common Playwright utilities:

### Usage in Your Tests

```javascript
const { 
  defaultConfig,
  delay,
  reloadPage,
  clickAt,
  checkScreenshot,
  login,
  waitForPageStability,
  clickOnPage,
  clickOnCommand,
  selectComboBox,
  fillTextInput,
  clickOnButton
} = require('./utils.js');

test('example test', async ({ page }) => {
  await login(page, 'username', 'password', defaultConfig, {
    username: ['LoginForm', 'UsernameInput'],
    password: ['LoginForm', 'PasswordInput'],
    submit: ['LoginForm', 'LoginButton']
  });
  
  await clickOnPage(page, 'Dashboard');
  await checkScreenshot(page, 'dashboard.png');
});
```

### Available Utilities

- **`defaultConfig`** - Default configuration with timeouts, base URL, screenshot settings
- **`delay(time)`** - Wait for specified milliseconds
- **`reloadPage(page, config, url)`** - Reload page and wait for stability
- **`clickAt(page, x, y, config)`** - Click at coordinates and wait for stability
- **`clickOnElement(page, path, config)`** - Click on element by objectName path
- **`clickOnPage(page, pageId, config)`** - Click on menu page button
- **`clickOnCommand(page, commandId, config)`** - Click on command button
- **`clickOnButton(page, buttonPath, config)`** - Click on button by path
- **`fillTextInput(page, text, path, config)`** - Fill text input field
- **`selectComboBox(page, selectedText, path, config)`** - Select combobox option
- **`waitForPageStability(page, config)`** - Wait for DOM to stabilize
- **`checkScreenshot(page, filename, masks, config)`** - Take and compare screenshot
- **`login(page, username, password, config, loginPaths)`** - Perform login

### Configuration Example

```javascript
const customConfig = {
  ...defaultConfig,
  baseUrl: process.env.BASE_URL || 'http://localhost:3000',
  timeouts: {
    delay: 500,
    locatorWait: 5000,
    stabilityTotal: 5000,
  },
};
```

**Windows script** (`run-tests-in-docker.ps1`):

```powershell
# Configuration
$ContainerName = "lisa-tests"
$ImageName = "imtcore-tests:windows"

# Start the container in detached mode
docker run -d `
  --name $ContainerName `
  -e BASE_URL="http://host.docker.internal:7776" `
  -e START_POSTGRESQL="true" `
  -e POSTGRES_DB="lisa_test" `
  -e DATABASE_URL="postgresql://postgres@localhost:5432/lisa_test" `
  -e TEST_USERNAME="su" `
  -e TEST_PASSWORD="1" `
  $ImageName `
  powershell -Command "Start-Sleep -Seconds 3600"

# Copy test files and resources
Write-Host "Copying test files..."
docker cp Tests/GUI "${ContainerName}:C:\app\tests\"
docker cp Tests/API "${ContainerName}:C:\app\tests\"

# Copy resources (installers, SQL scripts, etc.)
Write-Host "Copying resources..."
docker cp Tests/Docker/Resources/. "${ContainerName}:C:\app\custom-apps\resources\"

# Copy startup scripts
Write-Host "Copying startup scripts..."
docker cp Tests/Docker/Startup/. "${ContainerName}:C:\app\custom-apps\startup\"

# Restart container to run startup scripts and auto-detect tests
Write-Host "Running tests..."
docker restart $ContainerName
docker wait $ContainerName

# Copy test results out
Write-Host "Copying test results..."
docker cp "${ContainerName}:C:\app\tests\test-results" .\

# Cleanup
docker stop $ContainerName -ErrorAction SilentlyContinue
docker rm $ContainerName
```

## Resources Directory

The `Resources/` directory is where you place files needed for application installation:

- **Installers**: `.exe`, `.msi`, `.run`, `.deb`, `.rpm`, `.tar.gz`, `.zip`
- **SQL Scripts**: Database setup, migrations, seed data
- **Configuration Files**: `.json`, `.yaml`, `.xml`, `.ini`, `.conf`
- **Data Files**: Test data, reference data, backups

Files placed in `Tests/Docker/Resources/` in ImtCore are base resources. Application repositories should have their own `Tests/Docker/Resources/` directory with application-specific files.

**Container Paths:**
- Linux: `/app/custom-apps/resources/`
- Windows: `C:\app\custom-apps\resources\`

## Startup Directory

The `Startup/` directory contains scripts that run during container startup, after resources are copied.

### Script Naming Convention

Use numbered prefixes to control execution order:
- Linux: `01-setup-database.sh`, `02-start-application.sh`, `03-wait-for-services.sh`
- Windows: `01-setup-database.ps1`, `02-start-application.ps1`, `03-wait-for-services.ps1`

### Example Startup Scripts

**Linux** (`01-start-myapp.sh`):

```bash
#!/bin/bash
# 01-start-myapp.sh

set -e

echo "Starting MyApp..."

# Start the application
/opt/myapp/bin/myapp-server --port 3000 &

# Wait for application to be ready
for i in {1..30}; do
    if curl -f http://localhost:3000/health; then
        echo "MyApp is ready"
        exit 0
    fi
    echo "Waiting for MyApp... ($i/30)"
    sleep 2
done

echo "Failed to start MyApp"
exit 1
```

**Windows** (`01-start-myapp.ps1`):

```powershell
# 01-start-myapp.ps1

Write-Host "Starting MyApp..."

# Start the application
Start-Process -FilePath "C:\Program Files\MyApp\myapp-server.exe" -ArgumentList "--port", "3000"

# Wait for application to be ready
for ($i = 1; $i -le 30; $i++) {
    try {
        $response = Invoke-WebRequest -Uri "http://localhost:3000/health" -UseBasicParsing -ErrorAction SilentlyContinue
        if ($response.StatusCode -eq 200) {
            Write-Host "MyApp is ready"
            exit 0
        }
    } catch {
        # Service not ready yet
    }
    Write-Host "Waiting for MyApp... ($i/30)"
    Start-Sleep -Seconds 2
}

Write-Host "Failed to start MyApp"
exit 1
```

**Container Paths:**
- Linux: `/app/custom-apps/startup/`
- Windows: `C:\app\custom-apps\startup\`

## Startup Sequence

When the container starts, the entrypoint script executes in this order:

1. **PostgreSQL** (optional, if `START_POSTGRESQL=true`)
   - Initializes and starts PostgreSQL server
   - Creates database if `POSTGRES_DB` is set

2. **Startup Scripts** (from `/app/custom-apps/startup/`)
   - Runs in alphabetical order (use numbered prefixes)
   - Should start applications and wait for them to be ready

3. **Tests** (from the CMD or custom command)
   - Executes the test suite

## Environment Variables

Configure the container using these environment variables:

### Application Settings
- `BASE_URL` - URL of the application to test (default: `http://localhost:3000`)
- `CI` - CI mode indicator (default: `true`)

### PostgreSQL Settings
- `START_POSTGRESQL` - Start PostgreSQL in container (default: `false`)
- `POSTGRES_DB` - Database name to create (optional)
- `DATABASE_URL` - PostgreSQL connection string (optional)

### Custom Settings
Add your own environment variables as needed:
- `TEST_USERNAME`, `TEST_PASSWORD` - Test credentials
- Application-specific configuration

## See Also

- [apps-README.md](apps-README.md) - Documentation for custom applications
- [Resources/README.md](Resources/README.md) - Resources directory documentation
- Application repository (e.g., Lisa) - for application-specific test setup
