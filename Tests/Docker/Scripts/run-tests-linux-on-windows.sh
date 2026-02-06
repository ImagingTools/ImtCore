#!/bin/bash
# Wrapper script - calls the core script with default/example configuration
# For production use in application repos, copy run-tests-linux-on-windows-template.sh instead

# Set defaults if not already set
export IMAGE_NAME="${IMAGE_NAME:-imtcore-tests:linux}"
export START_POSTGRESQL="${START_POSTGRESQL:-true}"

# Call the core script from the same directory
SCRIPT_DIR="$(dirname "$0")"
bash "$SCRIPT_DIR/run-tests-linux-on-windows-core.sh"
exit $?
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

# Wait for container to finish (if entrypoint caused container to exit)
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