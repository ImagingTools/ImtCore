#!/bin/bash
# Example script for running tests using ImtCore Docker container

set -e

CONTAINER_NAME="myapp-tests-$(date +%s)"
IMAGE_NAME="imtcore-tests:linux"
BASE_URL="${BASE_URL:-http://localhost:3000}"
START_POSTGRESQL="${START_POSTGRESQL:-false}"
POSTGRES_DB="${POSTGRES_DB:-}"
DATABASE_URL="${DATABASE_URL:-}"
TEST_USERNAME="${TEST_USERNAME:-}"
TEST_PASSWORD="${TEST_PASSWORD:-}"

echo "=========================================="
echo "Running tests in ImtCore Docker container"
echo "=========================================="

echo "Starting container: $CONTAINER_NAME"
docker run -d \
  --name "$CONTAINER_NAME" \
  --network host \
  -e BASE_URL="$BASE_URL" \
  -e START_POSTGRESQL="$START_POSTGRESQL" \
  -e POSTGRES_DB="$POSTGRES_DB" \
  -e DATABASE_URL="$DATABASE_URL" \
  -e TEST_USERNAME="$TEST_USERNAME" \
  -e TEST_PASSWORD="$TEST_PASSWORD" \
  -e CI=true \
  -e PAUSE_BEFORE_TESTS=true \
  "$IMAGE_NAME"

sleep 2

if [ -d "Tests/GUI" ] && [ "$(ls -A Tests/GUI 2>/dev/null)" ]; then
    echo "Copying GUI tests..."
    docker cp Tests/GUI/. "$CONTAINER_NAME:/app/tests/GUI/"
fi

if [ -d "Tests/API" ] && [ "$(ls -A Tests/API 2>/dev/null)" ]; then
    echo "Copying API tests..."
    docker cp Tests/API/. "$CONTAINER_NAME:/app/tests/API/"
fi

if [ -d "Tests/Docker/Resources" ] && [ "$(ls -A Tests/Docker/Resources 2>/dev/null)" ]; then
    echo "Copying resources..."
    docker cp Tests/Docker/Resources/. "$CONTAINER_NAME:/app/custom-apps/resources/"
fi

if [ -d "Tests/Docker/Startup" ] && [ "$(ls -A Tests/Docker/Startup 2>/dev/null)" ]; then
    echo "Copying startup scripts..."
    docker cp Tests/Docker/Startup/. "$CONTAINER_NAME:/app/custom-apps/startup/"
    docker exec "$CONTAINER_NAME" chmod +x /app/custom-apps/startup/*.sh 2>/dev/null || true
fi

if [ -f "Tests/playwright.config.js" ]; then
    echo "Copying playwright.config.js..."
    docker cp Tests/playwright.config.js "$CONTAINER_NAME:/app/tests/"
fi

if [ -f "Tests/package.json" ]; then
    echo "Copying package.json..."
    docker cp Tests/package.json "$CONTAINER_NAME:/app/tests/"
    echo "Installing additional dependencies..."
    docker exec "$CONTAINER_NAME" npm install
fi

echo "Running tests..."
docker exec -e PAUSE_BEFORE_TESTS=false "$CONTAINER_NAME" /app/entrypoint.sh

docker wait "$CONTAINER_NAME" >/dev/null || true
EXIT_CODE=$(docker inspect "$CONTAINER_NAME" --format='{{.State.ExitCode}}')

echo "Copying test results..."
docker cp "$CONTAINER_NAME:/app/tests/test-results" ./test-results 2>/dev/null || echo "No test results to copy"

echo "=========================================="
echo "Container logs:"
echo "=========================================="
docker logs "$CONTAINER_NAME"

echo "Cleaning up..."
docker stop "$CONTAINER_NAME" 2>/dev/null || true
docker rm "$CONTAINER_NAME" 2>/dev/null || true

echo "=========================================="
if [ "$EXIT_CODE" -eq 0 ]; then
    echo "✓ Tests passed successfully!"
else
    echo "✗ Tests failed with exit code: $EXIT_CODE"
fi
echo "=========================================="

exit $EXIT_CODE