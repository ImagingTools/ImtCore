#!/bin/bash
# Build ImtCore Docker image for Linux
# This script builds the base test infrastructure that applications will use

set -e

echo "=========================================="
echo "Building ImtCore Docker Test Image (Linux)"
echo "=========================================="

# Configuration
IMAGE_NAME="${IMAGE_NAME:-imtcore-tests:linux}"
DOCKERFILE="Tests/Docker/Dockerfile.linux"

# Check if Dockerfile exists
if [ ! -f "$DOCKERFILE" ]; then
    echo "Error: Dockerfile not found at $DOCKERFILE"
    echo "Make sure you're running this script from the ImtCore repository root"
    exit 1
fi

echo "Building image: $IMAGE_NAME"
echo "Using Dockerfile: $DOCKERFILE"
echo ""

# Build the Docker image
docker build -f "$DOCKERFILE" -t "$IMAGE_NAME" .

if [ $? -eq 0 ]; then
    echo ""
    echo "=========================================="
    echo "✓ Build successful!"
    echo "=========================================="
    echo "Image: $IMAGE_NAME"
    echo ""
    echo "To verify the image:"
    echo "  docker images | grep imtcore-tests"
    echo ""
    echo "To use this image from an application repository:"
    echo "  1. Copy Tests/Docker/run-tests.sh to your application repo"
    echo "  2. Run: ./run-tests.sh"
    echo ""
else
    echo ""
    echo "=========================================="
    echo "✗ Build failed!"
    echo "=========================================="
    exit 1
fi
