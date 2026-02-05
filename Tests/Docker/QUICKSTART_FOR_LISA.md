# Quick Start Guide for Lisa Developers

This guide explains how to use the ImtCore Docker test infrastructure in the Lisa repository.

## What Was Implemented in ImtCore

ImtCore now provides a base Docker image with:
- Linux environment (Node.js 18 on Debian Bookworm)
- Browser testing dependencies (for Playwright, Selenium)
- PostgreSQL 16 (optional, can be enabled)
- Test execution infrastructure
- Startup script framework

## What Lisa Needs to Do

### 1. Create Directory Structure in Lisa

```bash
cd Lisa
mkdir -p Tests/Docker/Resources
mkdir -p Tests/Docker/Startup
```

### 2. Add Lisa-Specific Resources

Place in `Lisa/Tests/Docker/Resources/`:
- Lisa installer files (.exe, .run, etc.)
- Puma installer files
- SQL setup scripts (lisa-setup.sql, puma-setup.sql)
- Configuration files
- Database backups

Example:
```
Lisa/Tests/Docker/Resources/
├── lisa-installer.exe
├── puma-installer.run
├── lisa-setup.sql
├── puma-setup.sql
└── config.json
```

### 3. Create Startup Scripts

Place in `Lisa/Tests/Docker/Startup/`:

**01-reset-db.sh:**
```bash
#!/bin/bash
set -e

echo "Resetting database..."

# Drop and recreate database
psql -U postgres -c "DROP DATABASE IF EXISTS lisa_test;"
psql -U postgres -c "CREATE DATABASE lisa_test;"

# Run SQL setup
if [ -f "/app/custom-apps/resources/puma-setup.sql" ]; then
    psql -U postgres -d lisa_test -f /app/custom-apps/resources/puma-setup.sql
fi

if [ -f "/app/custom-apps/resources/lisa-setup.sql" ]; then
    psql -U postgres -d lisa_test -f /app/custom-apps/resources/lisa-setup.sql
fi

echo "Database ready"
```

**02-start-puma.sh:**
```bash
#!/bin/bash
set -e

echo "Starting Puma..."

# Start Puma application
/opt/puma/bin/puma-server --port 7788 &

# Wait for Puma to be ready
for i in {1..30}; do
    if curl -f http://localhost:7788/health 2>/dev/null; then
        echo "Puma is ready"
        exit 0
    fi
    echo "Waiting for Puma... ($i/30)"
    sleep 2
done

echo "Puma failed to start"
exit 1
```

**03-start-lisa.sh:**
```bash
#!/bin/bash
set -e

echo "Starting Lisa..."

# Start Lisa application
/opt/lisa/bin/lisa-server --port 7776 &

# Wait for Lisa to be ready
for i in {1..30}; do
    if curl -f http://localhost:7776/health 2>/dev/null; then
        echo "Lisa is ready"
        exit 0
    fi
    echo "Waiting for Lisa... ($i/30)"
    sleep 2
done

echo "Lisa failed to start"
exit 1
```

Make scripts executable:
```bash
chmod +x Tests/Docker/Startup/*.sh
```

### 4. Create Test Runner Script

Create `Lisa/run-tests-in-docker.sh`:

```bash
#!/bin/bash
set -e

# Configuration
CONTAINER_NAME="lisa-tests-$(date +%s)"
IMAGE_NAME="imtcore-tests:linux"

echo "Starting test container: $CONTAINER_NAME"

# Start the container in detached mode
docker run -d \
  --name "$CONTAINER_NAME" \
  --network host \
  -e BASE_URL="http://localhost:7776" \
  -e START_POSTGRESQL="true" \
  -e POSTGRES_DB="lisa_test" \
  -e DATABASE_URL="postgresql://postgres@localhost:5432/lisa_test" \
  -e PUMA_PORT="7788" \
  -e PUMA_URL="http://localhost:7788" \
  -e LISA_PORT="7776" \
  -e TEST_USERNAME="su" \
  -e TEST_PASSWORD="1" \
  "$IMAGE_NAME" \
  sleep infinity

# Copy test files and dependencies
echo "Copying test files..."
docker cp Tests/GUI "$CONTAINER_NAME:/app/tests/" || true
docker cp Tests/API "$CONTAINER_NAME:/app/tests/" || true
docker cp Tests/package.json "$CONTAINER_NAME:/app/tests/" || true
docker cp Tests/playwright.config.js "$CONTAINER_NAME:/app/tests/" || true

# Install test dependencies
echo "Installing test dependencies..."
docker exec "$CONTAINER_NAME" npm install

# Copy resources (installers, SQL scripts, etc.)
echo "Copying resources..."
docker cp Tests/Docker/Resources/. "$CONTAINER_NAME:/app/custom-apps/resources/" || true

# Copy startup scripts
echo "Copying startup scripts..."
docker cp Tests/Docker/Startup/. "$CONTAINER_NAME:/app/custom-apps/startup/" || true

# Make scripts executable
docker exec "$CONTAINER_NAME" chmod +x /app/custom-apps/startup/*.sh 2>/dev/null || true

# Run the startup scripts (they're automatically run by entrypoint)
echo "Restarting container to run startup scripts..."
docker restart "$CONTAINER_NAME"
sleep 5

# Run the tests
echo "Running tests..."
docker exec "$CONTAINER_NAME" npm test

# Copy test results out
echo "Copying test results..."
docker cp "$CONTAINER_NAME:/app/tests/test-results" ./ || true
docker cp "$CONTAINER_NAME:/app/tests/playwright-report" ./ || true

echo "Tests completed!"

# Cleanup
echo "Cleaning up container..."
docker stop "$CONTAINER_NAME"
docker rm "$CONTAINER_NAME"

echo "Done!"
```

Make it executable:
```bash
chmod +x run-tests-in-docker.sh
```

### 5. Build ImtCore Image

Before running Lisa tests, build the ImtCore image:

```bash
# Clone or update ImtCore
cd /path/to/ImtCore

# Build the image
docker build -f Tests/Docker/Dockerfile.linux -t imtcore-tests:linux .
```

### 6. Run Tests

```bash
cd /path/to/Lisa
./run-tests-in-docker.sh
```

## GitHub Actions Integration

Create `.github/workflows/docker-tests.yml` in Lisa:

```yaml
name: Docker Tests

on:
  push:
    branches: [ main, develop ]
  pull_request:
    branches: [ main, develop ]

jobs:
  test:
    runs-on: ubuntu-latest
    
    steps:
      - name: Checkout ImtCore
        uses: actions/checkout@v3
        with:
          repository: ImagingTools/ImtCore
          path: ImtCore
          
      - name: Checkout Lisa
        uses: actions/checkout@v3
        with:
          path: Lisa
          
      - name: Build ImtCore Docker Image
        run: |
          cd ImtCore
          docker build -f Tests/Docker/Dockerfile.linux -t imtcore-tests:linux .
          
      - name: Run Tests
        run: |
          cd Lisa
          chmod +x run-tests-in-docker.sh
          ./run-tests-in-docker.sh
          
      - name: Upload Test Results
        if: always()
        uses: actions/upload-artifact@v3
        with:
          name: test-results
          path: |
            Lisa/test-results/
            Lisa/playwright-report/
          retention-days: 30
```

## Troubleshooting

### Container fails to start
```bash
# Check logs
docker logs <container-name>

# Run interactively
docker run -it --rm imtcore-tests:linux /bin/bash
```

### PostgreSQL issues
```bash
# Check if PostgreSQL is running
docker exec <container-name> ps aux | grep postgres

# View PostgreSQL logs
docker exec <container-name> cat /var/log/postgresql/postgresql.log
```

### Startup scripts not running
```bash
# Check if scripts are executable
docker exec <container-name> ls -la /app/custom-apps/startup/

# Run script manually to debug
docker exec <container-name> /bin/bash /app/custom-apps/startup/01-reset-db.sh
```

### Tests not found
```bash
# Check what was copied
docker exec <container-name> ls -la /app/tests/

# Verify test files
docker exec <container-name> find /app/tests -name "*.spec.js"
```

## Summary

The workflow is:
1. ImtCore provides the base Docker image
2. Lisa copies its specific files into the running container
3. Startup scripts prepare the environment
4. Tests execute in the isolated container
5. Results are copied back to the host

This approach keeps the base infrastructure in ImtCore while allowing each application (Lisa, Puma, etc.) to customize as needed.
