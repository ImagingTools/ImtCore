# Docker Test Scripts

This directory contains scripts for building and running Docker-based tests across three different scenarios.

## Supported Scenarios

1. **Linux on Linux** - Native Linux containers on Linux host
2. **Linux on Windows** - Linux containers on Windows host (via Docker Desktop)
3. **Windows on Windows** - Native Windows containers on Windows host

## Structure

The scripts are organized into three categories:

### 1. Build Scripts
Scripts for building Docker images in ImtCore:
- `build-docker-linux.sh` - Build Linux container
- `build-docker-windows.bat` - Build Windows container
- `build-docker-linux-on-windows.bat` - Build Linux container on Windows (cmd)
- `build-docker-linux-on-windows.sh` - Build Linux container on Windows (Git Bash/WSL)

### 2. Core Test Runner Scripts (ImtCore only)
These scripts contain the core Docker orchestration logic and should **NOT** be copied to application repos:

**Linux on Linux:**
- `run-tests-linux-core.sh` - Core logic for native Linux

**Linux on Windows:**
- `run-tests-linux-on-windows-core.bat` - Core logic for Windows cmd
- `run-tests-linux-on-windows-core.sh` - Core logic for Git Bash/WSL

**Windows on Windows:**
- `run-tests-windows-core.bat` - Core logic for native Windows

### 3. Template/Wrapper Scripts
Two types of wrapper scripts:

#### A. Wrappers for ImtCore development (call core scripts with defaults)
- `run-tests-linux.sh` - Native Linux development
- `run-tests-linux-on-windows.bat` - Linux on Windows (cmd)
- `run-tests-linux-on-windows.sh` - Linux on Windows (bash)
- `run-tests-windows.bat` - Native Windows development

#### B. Templates for application repos (copy these to your app)
- `run-tests-linux-template.sh` - **Template for Linux on Linux**
- `run-tests-linux-on-windows-template.bat` - **Template for Linux on Windows (cmd)**
- `run-tests-linux-on-windows-template.sh` - **Template for Linux on Windows (bash)**
- `run-tests-windows-template.bat` - **Template for Windows on Windows**

## Usage for Application Repos (e.g., Lisa)

### Step 1: Choose Your Scenario

Pick the appropriate scenario for your development environment:

#### Scenario 1: Linux on Linux (Native Linux)
Use this on Linux hosts or Linux servers.
```bash
cp /path/to/ImtCore/Tests/Docker/Scripts/run-tests-linux-template.sh /path/to/Lisa/Tests/run-tests.sh
chmod +x /path/to/Lisa/Tests/run-tests.sh
```

#### Scenario 2: Linux on Windows (Docker Desktop)
Use this on Windows with Docker Desktop in Linux containers mode (most common for Windows developers).

**Windows Command Prompt:**
```batch
copy C:\path\to\ImtCore\Tests\Docker\Scripts\run-tests-linux-on-windows-template.bat C:\path\to\Lisa\Tests\run-tests.bat
```

**Git Bash / WSL:**
```bash
cp /path/to/ImtCore/Tests/Docker/Scripts/run-tests-linux-on-windows-template.sh /path/to/Lisa/Tests/run-tests.sh
chmod +x /path/to/Lisa/Tests/run-tests.sh
```

#### Scenario 3: Windows on Windows (Native Windows Containers)
Use this for testing native Windows applications in Windows containers.
```powershell
Copy-Item C:\path\to\ImtCore\Tests\Docker\Scripts\run-tests-windows-template.bat C:\path\to\Lisa\Tests\run-tests.bat
```

### Step 2: Configure the template

Edit your copied script and modify only the configuration section at the top:

#### Linux on Linux Configuration:
```bash
export BASE_URL="http://localhost:8080"  # Uses localhost via --network host
export START_POSTGRESQL="true"
export POSTGRES_DB="myapp_test"
export DATABASE_URL="postgresql://postgres:postgres@localhost:5432/myapp_test"
export TEST_USERNAME="test@example.com"
export TEST_PASSWORD="testpassword"
export IMAGE_NAME="imtcore-tests:linux"

# Update IMTCORE_PATH to point to your ImtCore location
IMTCORE_PATH="$(dirname "$0")/../../../../ImtCore"
```

#### Linux on Windows Configuration:
```batch
REM Windows Command Prompt
set BASE_URL=http://host.docker.internal:7776
set START_POSTGRESQL=true
set POSTGRES_DB=lisa_test
set DATABASE_URL=postgresql://postgres:postgres@localhost:5432/lisa_test
set TEST_USERNAME=admin@example.com
set TEST_PASSWORD=adminpassword

REM Update IMTCORE_PATH to point to your ImtCore location
set IMTCORE_PATH=%~dp0..\..\..\..\ImtCore
```

```bash
# Git Bash / WSL
export BASE_URL="http://host.docker.internal:7776"
export START_POSTGRESQL="true"
export POSTGRES_DB="lisa_test"
export DATABASE_URL="postgresql://postgres:postgres@localhost:5432/lisa_test"
export TEST_USERNAME="admin@example.com"
export TEST_PASSWORD="adminpassword"
export IMAGE_NAME="imtcore-tests:linux"

IMTCORE_PATH="$(dirname "$0")/../../../../ImtCore"
```

#### Windows on Windows Configuration:
```powershell
# PowerShell
$env:BASE_URL = "http://localhost:8080"
$env:START_POSTGRESQL = "false"  # PostgreSQL typically not available in Windows containers
$env:POSTGRES_DB = ""
$env:DATABASE_URL = ""
$env:TEST_USERNAME = "test@example.com"
$env:TEST_PASSWORD = "testpassword"
$env:IMAGE_NAME = "imtcore-tests:windows"

$IMTCORE_PATH = Join-Path (Split-Path $PSCommandPath) "..\..\..\..\ImtCore"
```

### Step 3: Run tests

#### Linux on Linux:
```bash
cd /path/to/Lisa/Tests
./run-tests.sh
```

#### Linux on Windows:
```batch
REM Windows Command Prompt
cd C:\path\to\Lisa\Tests
run-tests.bat
```

```bash
# Git Bash / WSL
cd /path/to/Lisa/Tests
./run-tests.sh
```

#### Windows on Windows:
```powershell
cd C:\path\to\Lisa\Tests
.\run-tests.bat
```

## Benefits of This Approach

1. **Separation of Concerns**: Complex Docker logic stays in ImtCore, applications only maintain configuration
2. **Easy Updates**: Bug fixes and improvements in ImtCore automatically benefit all applications
3. **Simple Configuration**: Application repos only need to set their specific variables
4. **Version Control**: Applications track only their configuration, not the complex logic
5. **Consistency**: All applications use the same tested Docker orchestration code

## Environment Variables

| Variable | Description | Default | Linux on Linux | Linux on Windows | Windows on Windows |
|----------|-------------|---------|----------------|------------------|--------------------|
| `BASE_URL` | URL of your application | - | `http://localhost:8080` | `http://host.docker.internal:7776` | `http://localhost:8080` |
| `START_POSTGRESQL` | Whether to start PostgreSQL in container | `true` | `true` or `false` | `true` or `false` | Usually `false` |
| `POSTGRES_DB` | PostgreSQL database name | - | `myapp_test` | `myapp_test` | N/A |
| `DATABASE_URL` | Full PostgreSQL connection string | - | `postgresql://...` | `postgresql://...` | N/A |
| `TEST_USERNAME` | Test user username/email | - | `test@example.com` | `test@example.com` | `test@example.com` |
| `TEST_PASSWORD` | Test user password | - | `testpassword` | `testpassword` | `testpassword` |
| `IMAGE_NAME` | Docker image to use | varies | `imtcore-tests:linux` | `imtcore-tests:linux` | `imtcore-tests:windows` |

### Key Differences by Scenario:

**Linux on Linux:**
- Uses `--network host` for direct localhost access
- BASE_URL uses `localhost` 
- No Docker Desktop mode check needed

**Linux on Windows:**
- Uses `--add-host=host.docker.internal:host-gateway`
- BASE_URL uses `host.docker.internal` to access Windows host
- Requires Docker Desktop in Linux containers mode

**Windows on Windows:**
- Uses native Windows networking
- BASE_URL uses `localhost`
- PostgreSQL typically not available (no good Windows container images)
- Windows paths (C:\app\) instead of Unix paths (/app/)

## Directory Structure Expected

```
YourApp/
├── Tests/
│   ├── run-tests.bat           # Your configured wrapper (from template)
│   ├── GUI/                    # Playwright tests
│   │   ├── *.spec.js
│   │   └── *.spec.ts
│   ├── API/                    # Postman collections
│   │   └── *.json
│   ├── Resources/              # Installation files (optional)
│   │   ├── setup.exe
│   │   └── schema.sql
│   └── Startup/                # Startup scripts (optional)
│       ├── 01-init-db.sh
│       └── 02-seed-data.sh
```

## Choosing the Right Scenario

| Scenario | When to Use | Pros | Cons |
|----------|-------------|------|------|
| **Linux on Linux** | Linux servers, CI/CD pipelines, Linux dev machines | Native performance, simple networking | Requires Linux host |
| **Linux on Windows** | Windows developers testing Linux apps | Cross-platform testing, most common setup | Requires Docker Desktop, slight overhead |
| **Windows on Windows** | Testing native Windows applications | Native Windows testing | Limited PostgreSQL support, less common |

## Troubleshooting

### "Cannot find ImtCore" error
Update the `IMTCORE_PATH` variable in your wrapper script to point to the correct ImtCore directory location.

### Docker not found
Ensure Docker (or Docker Desktop) is installed and running.

### Wrong container mode (Linux on Windows only)
For Linux containers on Windows, ensure Docker Desktop is set to "Linux containers" mode (not "Windows containers").
Right-click Docker Desktop system tray icon → "Switch to Linux containers"

### Connection refused to localhost (Linux on Windows)
Remember to use `host.docker.internal` instead of `localhost` in BASE_URL when accessing services on the Windows host.

### Tests not found
Ensure your Tests/GUI and Tests/API directories contain test files with correct naming:
- GUI: `*.spec.js` or `*.spec.ts`
- API: `*.json` (Postman collections)

### PowerShell execution policy (Windows on Windows)
If you get "execution policy" errors, run:
```powershell
Set-ExecutionPolicy -ExecutionPolicy RemoteSigned -Scope CurrentUser
```
