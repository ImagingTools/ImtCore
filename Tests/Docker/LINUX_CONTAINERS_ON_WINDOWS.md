# Running Linux Containers on Windows

This guide explains how to run ImtCore tests in Linux containers on Windows using Docker Desktop.

## Why Run Linux Containers on Windows?

- **Cross-platform testing**: Test your application in Linux environment without leaving Windows
- **CI/CD compatibility**: Match your local tests with Linux-based CI/CD pipelines (GitHub Actions, GitLab CI, etc.)
- **Better compatibility**: Many web frameworks and tools work better in Linux
- **Consistent environment**: Same container works on all platforms (Linux, macOS, Windows)
- **Faster builds**: Linux containers build faster (~5 min vs ~15 min for Windows containers)
- **Smaller images**: Linux images are much smaller (~1.5 GB vs ~5 GB for Windows containers)

## Prerequisites

1. **Windows 10/11 Pro, Enterprise, or Education** (or Windows Server)
   - Home edition works too, but requires WSL 2

2. **Docker Desktop for Windows** installed
   - Download from: https://www.docker.com/products/docker-desktop/
   - Make sure WSL 2 backend is enabled (default on modern installations)

3. **Hyper-V** or **WSL 2** enabled
   - Docker Desktop will prompt you to enable these if needed

## Step-by-Step Guide

### Step 1: Verify Docker Desktop Installation

Open PowerShell or Command Prompt and run:

```cmd
docker --version
```

You should see output like: `Docker version 24.0.x, build xxxxxxx`

### Step 2: Check Docker Desktop Mode

Docker Desktop can run in two modes:
- **Linux containers mode** (default) - what we need
- **Windows containers mode** - for Windows-specific containers

To check current mode:

```cmd
docker info | findstr "OSType"
```

**Expected output:** `OSType: linux`

If you see `OSType: windows`, switch to Linux mode:

1. Right-click Docker Desktop icon in system tray
2. Select **"Switch to Linux containers..."**
3. Wait for Docker to restart (may take 1-2 minutes)

### Step 3: Build the Linux Image

Navigate to your ImtCore repository:

```cmd
cd C:\path\to\ImtCore
```

Build the Linux container image:

```cmd
docker build -f Tests/Docker/Dockerfile.linux -t imtcore-tests:linux .
```

**Note:** First build takes ~5-10 minutes. Subsequent builds are much faster due to Docker caching.

### Step 4: Use from Your Application Repository

Navigate to your application repository (e.g., Lisa):

```cmd
cd C:\path\to\Lisa
```

Run tests using the Linux container. The exact command depends on your application's setup, but typically:

```cmd
docker run --rm ^
  --network host ^
  -e BASE_URL=http://localhost:7776 ^
  -e START_POSTGRESQL=true ^
  -e POSTGRES_DB=test_db ^
  imtcore-tests:linux ^
  npm test
```

## What Happens Behind the Scenes

When you run Linux containers on Windows:

1. **Docker Desktop** uses **WSL 2** (Windows Subsystem for Linux) backend
2. **Linux kernel** runs in a lightweight virtual machine
3. **Containers** run natively on that Linux kernel
4. **Performance** is near-native Linux performance
5. **Networking** is seamless between Windows host and Linux containers

## Complete Example

Here's a complete PowerShell script for running tests:

```powershell
# run-tests-linux-on-windows.ps1
# Run ImtCore Linux containers on Windows

$ErrorActionPreference = "Stop"

# Configuration
$ImtCorePath = "C:\path\to\ImtCore"
$LisaPath = "C:\path\to\Lisa"
$ContainerName = "lisa-tests-$(Get-Date -Format 'yyyyMMddHHmmss')"
$ImageName = "imtcore-tests:linux"

Write-Host "Building ImtCore Linux image..." -ForegroundColor Cyan
Push-Location $ImtCorePath
docker build -f Tests/Docker/Dockerfile.linux -t $ImageName .
if ($LASTEXITCODE -ne 0) {
    Write-Host "Build failed!" -ForegroundColor Red
    exit 1
}
Pop-Location

Write-Host "Starting Linux container on Windows..." -ForegroundColor Cyan
docker run -d `
  --name $ContainerName `
  --network host `
  -e BASE_URL="http://localhost:7776" `
  -e START_POSTGRESQL="true" `
  -e POSTGRES_DB="lisa_test" `
  $ImageName `
  sleep infinity

# Copy test files
Write-Host "Copying test files..." -ForegroundColor Cyan
Push-Location $LisaPath
docker cp Tests/GUI "${ContainerName}:/app/tests/"
docker cp Tests/API "${ContainerName}:/app/tests/"
docker cp Tests/Docker/Resources/. "${ContainerName}:/app/custom-apps/resources/"
docker cp Tests/Docker/Startup/. "${ContainerName}:/app/custom-apps/startup/"

# Make scripts executable
docker exec $ContainerName chmod +x /app/custom-apps/startup/*.sh

# Run tests
Write-Host "Running tests..." -ForegroundColor Cyan
docker exec $ContainerName npm test

# Copy results
Write-Host "Copying test results..." -ForegroundColor Cyan
docker cp "${ContainerName}:/app/tests/test-results" .\

# Cleanup
Write-Host "Cleaning up..." -ForegroundColor Cyan
docker stop $ContainerName
docker rm $ContainerName

Pop-Location
Write-Host "Done!" -ForegroundColor Green
```

## Troubleshooting

### Error: "Docker is not installed or not running"

**Solution:**
1. Install Docker Desktop from https://www.docker.com/products/docker-desktop/
2. Start Docker Desktop
3. Wait for it to fully start (icon will turn green)

### Error: "Docker Desktop appears to be in Windows containers mode"

**Solution:**
1. Right-click Docker Desktop tray icon
2. Select "Switch to Linux containers..."
3. Wait for Docker to restart
4. Run the script again

### Error: "Cannot connect to Docker daemon"

**Solution:**
1. Make sure Docker Desktop is running
2. Check system tray for Docker icon
3. Restart Docker Desktop if needed

### Build is very slow

**First build** is slow (5-10 minutes) because it downloads:
- Node.js base image (~300 MB)
- System dependencies
- PostgreSQL packages

**Subsequent builds** are much faster (1-2 minutes) due to Docker caching.

**Tip:** Keep Docker Desktop running in the background for faster startups.

### Tests fail inside container but pass locally

**Possible causes:**
1. **Different base URLs**: Check environment variables
2. **Network issues**: Container needs to access your app
3. **Missing dependencies**: Add them to Dockerfile.linux

**Debug steps:**
1. Check container logs: `docker logs <container-name>`
2. Access running container: `docker exec -it <container-name> /bin/bash`
3. Run tests manually inside container

### Path translation issues

Windows paths (e.g., `C:\Users\...`) are automatically translated to Linux paths inside the container. Use forward slashes in scripts that run inside the container.

## Comparison: Linux Containers vs Windows Containers

| Feature | Linux Containers on Windows | Windows Containers |
|---------|---------------------------|-------------------|
| **Host OS Required** | Windows 10/11 (any edition) | Windows 10/11 Pro+ or Windows Server |
| **Docker Mode** | Linux containers (default) | Windows containers |
| **Base Image** | Debian Linux | Windows Server Core |
| **Build Time** | ~5 min first, ~1 min later | ~15 min first, ~5 min later |
| **Image Size** | ~1.5 GB | ~5 GB |
| **Performance** | Fast (WSL 2 backend) | Slower (Windows overhead) |
| **CI/CD Compatible** | ✅ Works everywhere | ❌ Windows agents only |
| **Use Case** | General testing, CI/CD | Windows-specific testing |

**Recommendation:** Use Linux containers on Windows for most cases. They're faster, smaller, and work everywhere.

## Integration with CI/CD

Your local Linux container tests will match CI/CD environments:

### GitHub Actions

```yaml
name: Run Tests

on: [push, pull_request]

jobs:
  test:
    runs-on: ubuntu-latest  # Same Linux environment
    steps:
      - uses: actions/checkout@v3
      
      - name: Build ImtCore image
        run: |
          docker build -f Tests/Docker/Dockerfile.linux -t imtcore-tests:linux .
      
      - name: Run tests
        run: |
          # Your test commands here
          docker run --rm imtcore-tests:linux npm test
```

The **exact same Docker configuration** works locally on Windows and in CI/CD.

## Performance Tips

1. **Keep Docker Desktop running** - Reduces startup time
2. **Allocate more resources** - Docker Desktop Settings → Resources
   - Increase CPUs (4+ recommended)
   - Increase Memory (8GB+ recommended)
3. **Use SSD** - Docker performance depends heavily on disk speed
4. **Close other applications** - Free up system resources
5. **Enable BuildKit** - Faster Docker builds (enabled by default)

## Summary

Running Linux containers on Windows is fully supported and recommended:

✅ **Easy setup** - Use Docker Desktop's default Linux mode  
✅ **Fast performance** - Near-native speed with WSL 2  
✅ **Cross-platform** - Same containers work everywhere  
✅ **CI/CD compatible** - Match your pipeline environment  
✅ **Smaller & faster** - Better than Windows containers  

**Get Started:**

```powershell
# 1. Switch to Linux containers mode (if not already)
# 2. Build the image
cd C:\path\to\ImtCore
docker build -f Tests/Docker/Dockerfile.linux -t imtcore-tests:linux .

# 3. Use it from your application repository
cd C:\path\to\YourApp
# Run your tests...
```

---

**Happy Testing with Linux Containers on Windows! 🐧🪟**
