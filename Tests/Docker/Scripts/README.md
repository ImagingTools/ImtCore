# Scripts Directory

This directory contains all build and test execution scripts for the Docker test infrastructure.

## Build Scripts

Scripts to build Docker images in ImtCore:

- **`build-docker-linux.sh`** - Build Linux container
- **`build-docker-windows.ps1`** - Build Windows container
- **`build-docker-linux-on-windows.bat`** - Build Linux container on Windows (native cmd/PowerShell)
- **`build-docker-linux-on-windows.sh`** - Build Linux container on Windows (Git Bash/WSL)

### Usage

```bash
# From ImtCore root directory
./Tests/Docker/Scripts/build-docker-linux.sh
```

## Test Execution Scripts

Scripts to run tests from application repositories (e.g., Lisa):

- **`run-tests.sh`** - Run tests in Linux container
- **`run-tests.ps1`** - Run tests in Windows container
- **`run-tests-linux-on-windows.bat`** - Run Linux container tests on Windows (native cmd/PowerShell)
- **`run-tests-linux-on-windows.sh`** - Run Linux container tests on Windows (Git Bash/WSL)

### Usage

Copy the appropriate script to your application repository and execute it:

```bash
# Copy script
cp /path/to/ImtCore/Tests/Docker/Scripts/run-tests.sh .
chmod +x run-tests.sh

# Run tests
./run-tests.sh
```

## Script Features

All scripts include:
- ✅ Environment validation
- ✅ Docker mode checking (for Windows)
- ✅ Color-coded output
- ✅ Error handling
- ✅ Usage instructions
- ✅ Automatic test detection
- ✅ Results collection
