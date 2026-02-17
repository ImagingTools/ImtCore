#!/bin/bash
# Core test runner script for Linux containers on Windows
# This script should NOT be modified by application repos
# Applications should use a wrapper script that sets environment variables and calls this script

set -e

# Generate unique container name if not provided
CONTAINER_NAME="${CONTAINER_NAME:-myapp-tests-$(date +%s)}"

# Set constants and defaults
IMAGE_NAME="imtcore-tests:linux"
POSTGRES_PASSWORD="${POSTGRES_PASSWORD:-root}"
POSTGRES_DB="${POSTGRES_DB:-test_db}"

# Auto-construct DATABASE_URL
DATABASE_URL="postgresql://postgres:${POSTGRES_PASSWORD}@localhost:5432/${POSTGRES_DB}"

echo "=========================================="
echo "Running Linux containers on Windows"
echo "=========================================="
echo ""
echo "[DEBUG] Script path: $0"
echo "[DEBUG] Current dir: $(pwd)"
echo "[DEBUG] Container: $CONTAINER_NAME"
echo "[DEBUG] Image: $IMAGE_NAME"
echo "[DEBUG] BASE_URL: $BASE_URL"
echo "[DEBUG] POSTGRES_DB: $POSTGRES_DB"
echo "[DEBUG] POSTGRES_PASSWORD: $POSTGRES_PASSWORD"
echo "[DEBUG] DATABASE_URL: $DATABASE_URL"
echo "[DEBUG] TEST_USERS: $TEST_USERS"
echo "[DEBUG] UPDATE_SNAPSHOTS: ${UPDATE_SNAPSHOTS:-false}"
echo ""

if ! command -v docker &> /dev/null; then
    echo "ERROR: Docker is not installed or not in PATH"
    exit 1
fi

echo "Checking Docker Desktop mode..."
OS_TYPE=$(docker info --format '{{.OSType}}' 2>/dev/null || echo "unknown")

if [ "$OS_TYPE" != "linux" ]; then
    echo "ERROR: Docker Desktop is in Windows containers mode!"
    echo "Please switch to Linux containers mode"
    exit 1
fi

echo "✓ Docker Desktop is in Linux containers mode"
echo ""

if ! docker image inspect "$IMAGE_NAME" &> /dev/null; then
    echo "ERROR: Docker image '$IMAGE_NAME' not found"
    echo "Please build the image first using build-docker-linux-on-windows.sh"
    exit 1
fi

echo "[DEBUG] Host Tests directory structure:"
if [ -d "Tests/Startup" ]; then
    echo "[DEBUG] Host Startup *.sh:"
    ls -1 Tests/Startup/*.sh 2>/dev/null || echo "  (no .sh files)"
fi
if [ -d "Tests/GUI" ]; then
    echo "[DEBUG] Host GUI folder:"
    ls -1 Tests/GUI 2>/dev/null || echo "  (empty)"
fi
if [ -d "Tests/API" ]; then
    echo "[DEBUG] Host API folder:"
    ls -1 Tests/API 2>/dev/null || echo "  (empty)"
fi
echo ""

# Build volume mount arguments for application directories
VOLUME_MOUNTS=""

# Get ImtCore Docker directory (parent of Scripts folder where this script is located)
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
IMTCORE_DOCKER_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"

# Validate ImtCore entrypoint exists
if [ ! -f "$IMTCORE_DOCKER_DIR/entrypoint.sh" ]; then
    echo "ERROR: ImtCore entrypoint not found at: $IMTCORE_DOCKER_DIR/entrypoint.sh"
    exit 1
fi

echo "[DEBUG] Mounting ImtCore Docker directory from: $IMTCORE_DOCKER_DIR"

# Always mount ImtCore entrypoint script from ImtCore
IMTCORE_ENTRYPOINT="$IMTCORE_DOCKER_DIR/entrypoint.sh"
VOLUME_MOUNTS="$VOLUME_MOUNTS -v '$IMTCORE_ENTRYPOINT:/app/entrypoint.sh:ro'"

# Always mount ImtCore GUI utilities (utils.js, playwright.config.js, etc.) from ImtCore
IMTCORE_GUI_DIR="$IMTCORE_DOCKER_DIR/GUI"
if [ -d "$IMTCORE_GUI_DIR" ]; then
    VOLUME_MOUNTS="$VOLUME_MOUNTS -v '$IMTCORE_GUI_DIR:/app/tests/GUI:ro'"
    echo "[DEBUG] Mounting ImtCore GUI utilities from: $IMTCORE_GUI_DIR"
fi

# Mount application-specific test directories if they exist
if [ -d "Tests/GUI" ]; then
    VOLUME_MOUNTS="$VOLUME_MOUNTS -v '$PWD/Tests/GUI:/app/tests/GUI/app:ro'"
    echo "[DEBUG] Mounting application GUI tests from: Tests/GUI"
fi

if [ -d "Tests/API" ]; then
    VOLUME_MOUNTS="$VOLUME_MOUNTS -v '$PWD/Tests/API:/app/tests/API:ro'"
    echo "[DEBUG] Mounting application API tests from: Tests/API"
fi

if [ -d "Tests/Startup" ]; then
    VOLUME_MOUNTS="$VOLUME_MOUNTS -v '$PWD/Tests/Startup:/app/startup:ro'"
    echo "[DEBUG] Mounting application startup scripts from: Tests/Startup"
fi

if [ -d "Tests/Resources" ]; then
    VOLUME_MOUNTS="$VOLUME_MOUNTS -v '$PWD/Tests/Resources:/app/resources:ro'"
    echo "[DEBUG] Mounting application resources from: Tests/Resources"
fi

# Always mount test-results as read-write for output
mkdir -p "$PWD/test-results"
VOLUME_MOUNTS="$VOLUME_MOUNTS -v '$PWD/test-results:/app/tests/test-results'"
echo "[DEBUG] Mounting test results output to: test-results"

echo ""
echo "Starting container with volume mounts..."
eval docker run -d \
  --name "$CONTAINER_NAME" \
  --add-host=host.docker.internal:host-gateway \
  --entrypoint sh \
  -e BASE_URL="$BASE_URL" \
  -e START_POSTGRESQL=true \
  -e POSTGRES_PASSWORD="$POSTGRES_PASSWORD" \
  -e POSTGRES_DB="$POSTGRES_DB" \
  -e DATABASE_URL="$DATABASE_URL" \
  -e TEST_USERS="$TEST_USERS" \
  -e UPDATE_SNAPSHOTS="${UPDATE_SNAPSHOTS:-false}" \
  -e CI=true \
  $VOLUME_MOUNTS \
  "$IMAGE_NAME" \
  -c "sleep infinity"

if [ $? -ne 0 ]; then
    echo "ERROR: Failed to start container"
    exit 1
fi

docker ps -a --filter "name=$CONTAINER_NAME"

echo ""
echo "[DEBUG] Container mounted directories:"
docker exec "$CONTAINER_NAME" sh -c "
  echo '--- /app/startup ---'; 
  ls -la /app/startup 2>/dev/null || echo '(empty)'; 
  echo '--- /app/tests/GUI ---'; 
  ls -la /app/tests/GUI 2>/dev/null || echo '(empty)'; 
  echo '--- /app/tests/API ---'; 
  ls -la /app/tests/API 2>/dev/null || echo '(empty)'
"

echo ""
echo "Running tests (entrypoint)..."
set +e  # Don't exit on error, we want to capture the exit code
docker exec "$CONTAINER_NAME" bash /app/entrypoint.sh
EXIT_CODE=$?
set -e

echo "[DEBUG] Test run exit code: $EXIT_CODE"

echo ""
echo "Test results are available in: test-results/"

echo ""
echo "=========================================="
echo "Container logs:"
echo "=========================================="
docker logs "$CONTAINER_NAME"

echo ""
echo "Cleaning up container..."
docker rm -f "$CONTAINER_NAME" >/dev/null 2>&1

echo "=========================================="
if [ "$EXIT_CODE" -eq 0 ]; then
    echo "Tests passed successfully!"
else
    echo "Tests failed with exit code: $EXIT_CODE"
fi
echo "=========================================="

exit $EXIT_CODE
