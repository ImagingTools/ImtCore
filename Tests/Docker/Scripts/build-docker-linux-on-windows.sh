#!/bin/bash
# Build ImtCore Docker image for Linux containers running on Windows
# This script builds the Linux Docker image on Windows using Docker Desktop

set -e

echo "=========================================="
echo "Building ImtCore Docker Test Image"
echo "(Linux containers on Windows)"
echo "=========================================="

# Configuration
IMAGE_NAME="${IMAGE_NAME:-imtcore-tests:linux}"
DOCKERFILE="Tests/Docker/Dockerfile.linux"

# Check if running on Windows (WSL or Git Bash)
if ! command -v docker &> /dev/null; then
    echo "Error: Docker is not installed or not in PATH"
    exit 1
fi

# Check Docker mode
echo "Checking Docker Desktop mode..."
OS_TYPE=$(docker info --format '{{.OSType}}' 2>/dev/null)

if [ "$OS_TYPE" != "linux" ]; then
    echo ""
    echo "⚠️  Docker Desktop is in Windows containers mode!"
    echo ""
    echo "To build Linux containers on Windows:"
    echo "  1. Right-click Docker Desktop icon in system tray"
    echo "  2. Select 'Switch to Linux containers...'"
    echo "  3. Wait for Docker to restart"
    echo "  4. Run this script again"
    echo ""
    exit 1
fi

echo "✓ Docker Desktop is in Linux containers mode"
echo ""

# Check if Dockerfile exists
if [ ! -f "$DOCKERFILE" ]; then
    echo "Error: Dockerfile not found at $DOCKERFILE"
    echo "Make sure you're running this script from the ImtCore repository root"
    exit 1
fi

echo "Building Linux image on Windows..."
echo "Image: $IMAGE_NAME"
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
    echo "This Linux container can now run on Windows via Docker Desktop (WSL 2)"
    echo ""
    echo "To verify the image:"
    echo "  docker images | grep imtcore-tests"
    echo ""
    echo "To use this image from an application repository:"
    echo "  1. Copy Tests/Docker/run-tests.sh to your application repo"
    echo "  2. Run: ./run-tests.sh"
    echo ""
    echo "Note: Linux containers on Windows use WSL 2 backend"
    echo "      This provides near-native Linux performance"
    echo ""
else
    echo ""
    echo "=========================================="
    echo "✗ Build failed!"
    echo "=========================================="
    exit 1
fi
