#!/bin/bash
# Simple configuration file for running Docker tests (Linux containers on Linux)
# Copy this file to your application's Tests directory and configure the variables below

# ==========================================
# CONFIGURATION
# ==========================================

# Application URL (use localhost since we're using --network host)
BASE_URL="http://localhost:8080"

# PostgreSQL database name
POSTGRES_DB="myapp_test"

# PostgreSQL password (default: root)
POSTGRES_PASSWORD="root"

# Test credentials
TEST_USERNAME="test@example.com"
TEST_PASSWORD="testpassword"

# ==========================================
# DO NOT MODIFY BELOW THIS LINE
# ==========================================

# Get script directory (should be Tests/)
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Set container details
IMAGE_NAME="imtcore-tests:linux"
CONTAINER_NAME="myapp-tests-$(date +%s)"

# Auto-construct DATABASE_URL
DATABASE_URL="postgresql://postgres:${POSTGRES_PASSWORD}@localhost:5432/${POSTGRES_DB}"

echo "==========================================" 
echo "Running Linux containers on Linux"
echo "=========================================="
echo "Container: $CONTAINER_NAME"
echo "Image: $IMAGE_NAME"
echo "BASE_URL: $BASE_URL"
echo ""

# Check if Docker is installed
if ! command -v docker &> /dev/null; then
    echo "ERROR: Docker is not installed or not in PATH"
    exit 1
fi

# Check if image exists
if ! docker image inspect "$IMAGE_NAME" >/dev/null 2>&1; then
    echo "ERROR: Docker image '$IMAGE_NAME' not found"
    echo "Please build the image first using ImtCore build scripts"
    exit 1
fi

# Run container with volume mounts and host networking
echo "Starting container with test files..."
docker run --rm \
    --name "$CONTAINER_NAME" \
    --network host \
    -e BASE_URL="$BASE_URL" \
    -e POSTGRES_DB="$POSTGRES_DB" \
    -e POSTGRES_PASSWORD="$POSTGRES_PASSWORD" \
    -e DATABASE_URL="$DATABASE_URL" \
    -e TEST_USERNAME="$TEST_USERNAME" \
    -e TEST_PASSWORD="$TEST_PASSWORD" \
    -v "$SCRIPT_DIR/GUI:/mnt/app/GUI:ro" \
    -v "$SCRIPT_DIR/API:/mnt/app/API:ro" \
    -v "$SCRIPT_DIR/../Resources:/mnt/app/Resources:ro" \
    -v "$SCRIPT_DIR/../Startup:/mnt/app/Startup:ro" \
    -v "$SCRIPT_DIR/package.json:/mnt/app/package.json:ro" \
    "$IMAGE_NAME" \
    /app/copy-and-run.sh

EXIT_CODE=$?

echo ""
echo "=========================================="
echo "Tests completed with exit code: $EXIT_CODE"
echo "=========================================="

exit $EXIT_CODE
