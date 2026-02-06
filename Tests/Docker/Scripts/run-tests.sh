#!/bin/bash
# Run tests using Linux containers

set -e

CONTAINER_NAME="myapp-tests-$$-$RANDOM"
if [ -z "$IMAGE_NAME" ]; then
    IMAGE_NAME="imtcore-tests:linux"
fi
# IMPORTANT: localhost inside container == container itself; use localhost for host apps on Linux
BASE_URL="${BASE_URL:-}"
START_POSTGRESQL="${START_POSTGRESQL:-true}"
POSTGRES_DB="${POSTGRES_DB:-}"
DATABASE_URL="${DATABASE_URL:-}"
TEST_USERNAME="${TEST_USERNAME:-}"
TEST_PASSWORD="${TEST_PASSWORD:-}"

echo "=========================================="
echo "Running Linux containers"
echo "=========================================="
echo ""
echo "[DEBUG] Script path : $0"
echo "[DEBUG] Current dir : $(pwd)"
echo "[DEBUG] Container   : $CONTAINER_NAME"
echo "[DEBUG] Image       : $IMAGE_NAME"
echo "[DEBUG] BASE_URL    : $BASE_URL"
echo ""

# Check if Docker is installed
if ! command -v docker &> /dev/null; then
    echo "Docker not found in PATH"
    exit 1
fi

# Check if image exists
if ! docker image inspect "$IMAGE_NAME" > /dev/null 2>&1; then
    echo "Image not found: $IMAGE_NAME"
    exit 1
fi

echo ""
echo "[DEBUG] Host Startup *.sh:"
ls -1 Tests/Startup/*.sh 2>/dev/null || true
echo "[DEBUG] Host GUI folder:"
ls -1 Tests/GUI 2>/dev/null || true
echo "[DEBUG] Host API folder:"
ls -1 Tests/API 2>/dev/null || true
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
    echo "Failed to start container"
    exit 1
fi

docker ps -a --filter "name=$CONTAINER_NAME"

# Ensure target dirs exist
docker exec "$CONTAINER_NAME" sh -lc "mkdir -p /app/tests/GUI /app/tests/API /app/startup /app/resources"

echo ""
echo "Copying GUI tests..."
if [ -d "Tests/GUI" ]; then
    docker cp "Tests/GUI/." "$CONTAINER_NAME:/app/tests/GUI/"
fi

echo "Copying API tests..."
if [ -d "Tests/API" ]; then
    docker cp "Tests/API/." "$CONTAINER_NAME:/app/tests/API/"
fi

echo "Copying resources..."
if [ -d "Tests/Resources" ]; then
    docker cp "Tests/Resources/." "$CONTAINER_NAME:/app/resources/"
fi

echo "Copying startup scripts..."
if [ -d "Tests/Startup" ]; then
    docker cp "Tests/Startup/." "$CONTAINER_NAME:/app/startup/"
    docker exec "$CONTAINER_NAME" sh -lc "chmod +x /app/startup/*.sh 2>/dev/null || true"
fi

echo ""
echo "[DEBUG] Container files after copy:"
docker exec "$CONTAINER_NAME" sh -lc "echo '--- /app/startup ---'; ls -la /app/startup; echo '--- /app/startup/*.sh ---'; ls -la /app/startup/*.sh 2>/dev/null || true; echo '--- /app/tests/GUI ---'; ls -la /app/tests/GUI 2>/dev/null || true; echo '--- /app/tests/API ---'; ls -la /app/tests/API 2>/dev/null || true"

echo ""
echo "Running tests (entrypoint)..."
docker exec "$CONTAINER_NAME" sh -lc "export PAUSE_BEFORE_TESTS=false; /app/entrypoint.sh"
EXIT_CODE=$?
echo "[DEBUG] Test run exit code: $EXIT_CODE"

RESULTS_DIR="$(dirname "$0")/test-results"

echo "Copying test results..."
if [ -d "$RESULTS_DIR" ]; then
    rm -rf "$RESULTS_DIR"
fi
docker cp "$CONTAINER_NAME:/app/tests/test-results" "$RESULTS_DIR" 2>/dev/null || echo "No test results to copy"

echo "=========================================="
echo "Container logs:"
echo "=========================================="
docker logs "$CONTAINER_NAME"

echo "Cleaning up..."
docker rm -f "$CONTAINER_NAME" >/dev/null 2>&1

echo "=========================================="
if [ "$EXIT_CODE" -eq 0 ]; then
    echo "Tests passed successfully"
else
    echo "Tests failed with exit code: $EXIT_CODE"
fi
echo "=========================================="

exit $EXIT_CODE