# Docker Test Scripts

This directory contains scripts for building and running Docker-based tests.

## Structure

The scripts are organized into three categories:

### 1. Build Scripts
Scripts for building Docker images in ImtCore:
- `build-docker-linux.sh` - Build Linux container
- `build-docker-windows.ps1` - Build Windows container
- `build-docker-linux-on-windows.bat` - Build Linux container on Windows (cmd)
- `build-docker-linux-on-windows.sh` - Build Linux container on Windows (Git Bash/WSL)

### 2. Core Test Runner Scripts (ImtCore only)
These scripts contain the core Docker orchestration logic and should **NOT** be copied to application repos:
- `run-tests-linux-on-windows-core.bat` - Core logic for Windows cmd
- `run-tests-linux-on-windows-core.sh` - Core logic for Git Bash/WSL

### 3. Template/Wrapper Scripts
Two types of wrapper scripts:

#### A. Wrappers for ImtCore development (call core scripts with defaults)
- `run-tests-linux-on-windows.bat` 
- `run-tests-linux-on-windows.sh`
- `run-tests.sh`
- `run-tests.ps1`

#### B. Templates for application repos (copy these to your app)
- `run-tests-linux-on-windows-template.bat` - **Copy this to your app repo**
- `run-tests-linux-on-windows-template.sh` - **Copy this to your app repo**

## Usage for Application Repos (e.g., Lisa)

### Step 1: Copy the template script

Copy the appropriate template to your application's `Tests/` directory:

**For Linux containers on Windows (recommended):**
```batch
REM Windows Command Prompt
copy C:\path\to\ImtCore\Tests\Docker\Scripts\run-tests-linux-on-windows-template.bat C:\path\to\Lisa\Tests\run-tests.bat
```

```bash
# Git Bash / WSL
cp /path/to/ImtCore/Tests/Docker/Scripts/run-tests-linux-on-windows-template.sh /path/to/Lisa/Tests/run-tests.sh
chmod +x /path/to/Lisa/Tests/run-tests.sh
```

### Step 2: Configure the template

Edit your copied `run-tests.bat` or `run-tests.sh` and modify only the configuration section at the top:

```batch
REM Example configuration for Lisa
set BASE_URL=http://host.docker.internal:7776
set START_POSTGRESQL=true
set POSTGRES_DB=lisa_test
set DATABASE_URL=postgresql://postgres:postgres@localhost:5432/lisa_test
set TEST_USERNAME=admin@example.com
set TEST_PASSWORD=adminpassword

REM Update IMTCORE_PATH to point to your ImtCore location
set IMTCORE_PATH=%~dp0..\..\..\..\ImtCore
```

### Step 3: Run tests

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

## Benefits of This Approach

1. **Separation of Concerns**: Complex Docker logic stays in ImtCore, applications only maintain configuration
2. **Easy Updates**: Bug fixes and improvements in ImtCore automatically benefit all applications
3. **Simple Configuration**: Application repos only need to set their specific variables
4. **Version Control**: Applications track only their configuration, not the complex logic
5. **Consistency**: All applications use the same tested Docker orchestration code

## Environment Variables

| Variable | Description | Default | Example |
|----------|-------------|---------|---------|
| `BASE_URL` | URL of your application (use `host.docker.internal` for Windows host) | - | `http://host.docker.internal:7776` |
| `START_POSTGRESQL` | Whether to start PostgreSQL in container | `true` | `true` or `false` |
| `POSTGRES_DB` | PostgreSQL database name | - | `myapp_test` |
| `DATABASE_URL` | Full PostgreSQL connection string | - | `postgresql://user:pass@localhost:5432/db` |
| `TEST_USERNAME` | Test user username/email | - | `test@example.com` |
| `TEST_PASSWORD` | Test user password | - | `testpassword` |
| `IMAGE_NAME` | Docker image to use | `imtcore-tests:linux` | `imtcore-tests:linux` |

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

## Troubleshooting

### "Cannot find ImtCore" error
Update the `IMTCORE_PATH` variable in your wrapper script to point to the correct ImtCore directory location.

### Docker not found
Ensure Docker Desktop is installed and running.

### Wrong container mode
For Linux containers on Windows, ensure Docker Desktop is set to "Linux containers" mode (not "Windows containers").

### Tests not found
Ensure your Tests/GUI and Tests/API directories contain test files with correct naming:
- GUI: `*.spec.js` or `*.spec.ts`
- API: `*.json` (Postman collections)
