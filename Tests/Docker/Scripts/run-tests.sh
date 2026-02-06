#!/bin/bash
# Example script for running tests using ImtCore Docker container
# This script is meant to be used from application repositories (e.g., Lisa)
# 
# Usage:
#   1. Build ImtCore Docker image first (in ImtCore repository)
#   2. Run this script from your application repository
#   3. Tests will be executed automatically in the container

set -e

# Configuration
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

# Start the container in detached mode
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
  "$IMAGE_NAME" \
  sleep infinity

# Wait a bit for container to be ready
sleep 2

# Copy GUI tests (Playwright)
if [ -d "Tests/GUI" ] && [ "$(ls -A Tests/GUI 2>/dev/null)" ]; then
    echo "Copying GUI tests..."
    docker cp Tests/GUI/. "$CONTAINER_NAME:/app/tests/GUI/"
fi

# Copy API tests (Postman)
if [ -d "Tests/API" ] && [ "$(ls -A Tests/API 2>/dev/null)" ]; then
    echo "Copying API tests..."
    docker cp Tests/API/. "$CONTAINER_NAME:/app/tests/API/"
fi

# Copy resources (installers, SQL scripts, configs, etc.)
if [ -d "Tests/Docker/Resources" ] && [ "$(ls -A Tests/Docker/Resources 2>/dev/null)" ]; then
    echo "Copying resources..."
    docker cp Tests/Docker/Resources/. "$CONTAINER_NAME:/app/custom-apps/resources/"
fi

# Copy startup scripts
if [ -d "Tests/Docker/Startup" ] && [ "$(ls -A Tests/Docker/Startup 2>/dev/null)" ]; then
    echo "Copying startup scripts..."
    docker cp Tests/Docker/Startup/. "$CONTAINER_NAME:/app/custom-apps/startup/"
    # Make scripts executable
    docker exec "$CONTAINER_NAME" chmod +x /app/custom-apps/startup/*.sh 2>/dev/null || true
fi

# Copy playwright.config.js if exists
if [ -f "Tests/playwright.config.js" ]; then
    echo "Copying playwright.config.js..."
    docker cp Tests/playwright.config.js "$CONTAINER_NAME:/app/tests/"
fi

# Copy package.json if exists (for additional dependencies)
if [ -f "Tests/package.json" ]; then
    echo "Copying package.json..."
    docker cp Tests/package.json "$CONTAINER_NAME:/app/tests/"
    echo "Installing additional dependencies..."
    docker exec "$CONTAINER_NAME" npm install
fi

# Restart container to trigger automatic test detection and execution
echo "Running tests..."
docker restart "$CONTAINER_NAME"

# Wait for container to finish
docker wait "$CONTAINER_NAME"

# Get exit code
EXIT_CODE=$(docker inspect "$CONTAINER_NAME" --format='{{.State.ExitCode}}')

# Copy test results out
echo "Copying test results..."
docker cp "$CONTAINER_NAME:/app/tests/test-results" ./test-results 2>/dev/null || echo "No test results to copy"

# Show logs
echo "=========================================="
echo "Container logs:"
echo "=========================================="
docker logs "$CONTAINER_NAME"

# Cleanup
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
