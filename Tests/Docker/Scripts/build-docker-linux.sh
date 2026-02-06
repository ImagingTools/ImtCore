#!/bin/bash
# Build ImtCore Docker image for Linux
# This script builds the base test infrastructure that applications will use

set -e

echo "=========================================="
echo "Building ImtCore Docker Test Image"
echo "(Linux)"
echo "=========================================="
echo ""

# Configuration
if [ -z "$IMAGE_NAME" ]; then
    IMAGE_NAME="imtcore-tests:linux"
fi
DOCKERFILE="Tests/Docker/Dockerfile.linux"

# Check if Docker is installed
if ! command -v docker &> /dev/null; then
    echo "Error: Docker is not installed or not in PATH"
    exit 1
fi

# Check if Dockerfile exists
if [ ! -f "$DOCKERFILE" ]; then
    echo "Error: Dockerfile not found at $DOCKERFILE"
    echo "Make sure you're running this script from the ImtCore repository root"
    exit 1
fi

echo "Building Linux image..."
echo "Image: $IMAGE_NAME"
echo "Using Dockerfile: $DOCKERFILE"
echo ""

# Build the Docker image
docker build -f "$DOCKERFILE" -t "$IMAGE_NAME" .

if [ $? -eq 0 ]; then
    echo ""
    echo "=========================================="
    echo "[SUCCESS] Build successful!"
    echo "=========================================="
    echo "Image: $IMAGE_NAME"
    echo ""
    echo "To verify the image:"
    echo "  docker images | grep imtcore-tests"
    echo ""
    echo "To use this image from an application repository:"
    echo "  1. Copy Tests/Docker/Scripts/run-tests.sh to your application repo"
    echo "  2. Run: ./run-tests.sh"
    echo ""
else
    echo ""
    echo "=========================================="
    echo "[FAILED] Build failed!"
    echo "=========================================="
    exit 1
fi
