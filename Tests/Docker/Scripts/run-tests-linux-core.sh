#!/bin/bash
# Core test runner script for Linux containers on Linux
# This script should NOT be modified by application repos
# Applications should use a wrapper script that sets environment variables and calls this script

set -e

# Generate unique container name if not provided
CONTAINER_NAME="${CONTAINER_NAME:-myapp-tests-$(date +%s)}"

# Set defaults for optional variables
IMAGE_NAME="${IMAGE_NAME:-imtcore-tests:linux}"
START_POSTGRESQL="${START_POSTGRESQL:-true}"

echo "=========================================="
echo "Running Linux containers on Linux"
echo "=========================================="
echo ""
echo "[DEBUG] Script path: $0"
echo "[DEBUG] Current dir: $(pwd)"
echo "[DEBUG] Container: $CONTAINER_NAME"
echo "[DEBUG] Image: $IMAGE_NAME"
echo "[DEBUG] BASE_URL: $BASE_URL"
echo "[DEBUG] START_POSTGRESQL: $START_POSTGRESQL"
echo "[DEBUG] POSTGRES_DB: $POSTGRES_DB"
echo "[DEBUG] DATABASE_URL: $DATABASE_URL"
echo "[DEBUG] TEST_USERNAME: $TEST_USERNAME"
echo "[DEBUG] TEST_PASSWORD: $TEST_PASSWORD"
echo ""

if ! command -v docker &> /dev/null; then
    echo "ERROR: Docker is not installed or not in PATH"
    exit 1
fi

if ! docker image inspect "$IMAGE_NAME" &> /dev/null; then
    echo "ERROR: Docker image '$IMAGE_NAME' not found"
    echo "Please build the image first using build-docker-linux.sh"
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

echo "Starting idle container (entrypoint will NOT run yet)..."
docker run -d \
  --name "$CONTAINER_NAME" \
  --network host \
  --entrypoint sh \
  -e BASE_URL="$BASE_URL" \
  -e START_POSTGRESQL="$START_POSTGRESQL" \
  -e POSTGRES_DB="$POSTGRES_DB" \
  -e DATABASE_URL="$DATABASE_URL" \
  -e TEST_USERNAME="$TEST_USERNAME" \
  -e TEST_PASSWORD="$TEST_PASSWORD" \
  -e CI=true \
  "$IMAGE_NAME" \
  -lc "sleep infinity"

if [ $? -ne 0 ]; then
    echo "ERROR: Failed to start container"
    exit 1
fi

docker ps -a --filter "name=$CONTAINER_NAME"

# Ensure target dirs exist
echo ""
echo "Creating directories in container..."
docker exec "$CONTAINER_NAME" sh -lc "mkdir -p /app/tests/GUI /app/tests/API /app/startup /app/resources"

echo ""
echo "Copying GUI tests..."
if [ -d "Tests/GUI" ]; then
    docker cp "Tests/GUI/." "$CONTAINER_NAME:/app/tests/GUI/"
else
    echo "WARNING: Tests/GUI directory not found - skipping"
fi

echo "Copying API tests..."
if [ -d "Tests/API" ]; then
    docker cp "Tests/API/." "$CONTAINER_NAME:/app/tests/API/"
else
    echo "WARNING: Tests/API directory not found - skipping"
fi

echo "Copying resources..."
if [ -d "Tests/Resources" ]; then
    docker cp "Tests/Resources/." "$CONTAINER_NAME:/app/resources/"
else
    echo "WARNING: Tests/Resources directory not found - skipping"
fi

echo "Copying startup scripts..."
if [ -d "Tests/Startup" ]; then
    docker cp "Tests/Startup/." "$CONTAINER_NAME:/app/startup/"
    docker exec "$CONTAINER_NAME" sh -lc "chmod +x /app/startup/*.sh 2>/dev/null || true"
else
    echo "WARNING: Tests/Startup directory not found - skipping"
fi

echo ""
echo "[DEBUG] Container files after copy:"
docker exec "$CONTAINER_NAME" sh -lc "
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
docker exec "$CONTAINER_NAME" sh -lc "export PAUSE_BEFORE_TESTS=false; /app/entrypoint.sh"
EXIT_CODE=$?
set -e

echo "[DEBUG] Test run exit code: $EXIT_CODE"

RESULTS_DIR="$(dirname "$0")/test-results"

echo ""
echo "Copying test results..."
rm -rf "$RESULTS_DIR" 2>/dev/null || true
docker cp "$CONTAINER_NAME:/app/tests/test-results" "$RESULTS_DIR" 2>/dev/null || echo "No test results to copy"

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
