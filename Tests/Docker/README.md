# ImtCore Docker Test Environment

This directory contains the Docker infrastructure for running tests in a containerized environment. This is the **base infrastructure** that should be used by application repositories (like Lisa) to run their tests.

## Overview

The test execution flow is:

1. **Build Docker image** from ImtCore - creates the base test environment
2. **Run container** from application repository (e.g., Lisa) 
3. **Copy files** from application repository into the running container:
   - GUI tests (Playwright, Selenium, etc.)
   - API tests (Postman, REST Client, etc.)
   - Resources (installers, SQL scripts, config files)
   - Startup scripts (application initialization)
4. **Execute tests** in the container

## Directory Structure

```
Tests/Docker/
├── Dockerfile.linux          # Linux container configuration
├── docker-compose.linux.yml  # Linux compose configuration
├── entrypoint.sh             # Container startup script
├── .dockerignore             # Files to exclude from build
├── Resources/                # Base resources from ImtCore
│   ├── .gitkeep             # Keeps directory in git
│   └── README.md            # Resources documentation
├── Startup/                  # Base startup scripts from ImtCore
│   └── .gitkeep             # Keeps directory in git
├── apps-README.md           # Documentation for custom apps
└── README.md                # This file
```

## Container Structure

When the container is built, it creates the following structure:

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

## Building the Docker Image

From the ImtCore repository root:

```bash
# Build the image
docker build -f Tests/Docker/Dockerfile.linux -t imtcore-tests:linux .

# Or using docker-compose
docker-compose -f Tests/Docker/docker-compose.linux.yml build
```

## Using from Application Repository (e.g., Lisa)

### Step 1: Build the Base Image

From your ImtCore repository:
```bash
docker build -f Tests/Docker/Dockerfile.linux -t imtcore-tests:linux .
```

### Step 2: Create Run Script in Your Application Repository

Create a script in your application repository (e.g., `run-tests-in-docker.sh`):

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

# Run the tests
echo "Running tests..."
docker exec "$CONTAINER_NAME" npm test

# Copy test results out
echo "Copying test results..."
docker cp "$CONTAINER_NAME:/app/tests/test-results" ./

# Cleanup
docker stop "$CONTAINER_NAME"
docker rm "$CONTAINER_NAME"
```

## Resources Directory

The `Resources/` directory is where you place files needed for application installation:

- **Installers**: `.exe`, `.msi`, `.run`, `.deb`, `.rpm`, `.tar.gz`, `.zip`
- **SQL Scripts**: Database setup, migrations, seed data
- **Configuration Files**: `.json`, `.yaml`, `.xml`, `.ini`, `.conf`
- **Data Files**: Test data, reference data, backups

Files placed in `Tests/Docker/Resources/` in ImtCore are base resources. Application repositories should have their own `Tests/Docker/Resources/` directory with application-specific files.

**Container Path**: `/app/custom-apps/resources/`

## Startup Directory

The `Startup/` directory contains scripts that run during container startup, after resources are copied.

### Script Naming Convention

Use numbered prefixes to control execution order:
- `01-setup-database.sh` - Database initialization
- `02-start-application.sh` - Start your application
- `03-wait-for-services.sh` - Wait for services to be ready

### Example Startup Script

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

**Container Path**: `/app/custom-apps/startup/`

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
