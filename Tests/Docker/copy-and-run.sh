#!/bin/bash
# This script runs inside the Docker container
# It copies test files from mounted volumes and then runs the entrypoint

set -e

echo "=========================================="
echo "Docker Test Runner - File Copy & Execute"
echo "=========================================="

# Copy GUI tests if mounted
if [ -d "/mnt/app/GUI" ]; then
    echo "Copying GUI tests from /mnt/app/GUI to /app/tests/GUI..."
    mkdir -p /app/tests/GUI
    cp -r /mnt/app/GUI/* /app/tests/GUI/ 2>/dev/null || true
    echo "✓ GUI tests copied"
fi

# Copy API tests if mounted  
if [ -d "/mnt/app/API" ]; then
    echo "Copying API tests from /mnt/app/API to /app/tests/API..."
    mkdir -p /app/tests/API
    cp -r /mnt/app/API/* /app/tests/API/ 2>/dev/null || true
    echo "✓ API tests copied"
fi

# Copy resources if mounted
if [ -d "/mnt/app/Resources" ]; then
    echo "Copying resources from /mnt/app/Resources to /app/resources..."
    mkdir -p /app/resources
    cp -r /mnt/app/Resources/* /app/resources/ 2>/dev/null || true
    echo "✓ Resources copied"
fi

# Copy startup scripts if mounted
if [ -d "/mnt/app/Startup" ]; then
    echo "Copying startup scripts from /mnt/app/Startup to /app/startup..."
    mkdir -p /app/startup
    cp -r /mnt/app/Startup/* /app/startup/ 2>/dev/null || true
    echo "✓ Startup scripts copied"
fi

# Install additional dependencies if package.json exists
if [ -f "/mnt/app/package.json" ]; then
    echo "Installing additional dependencies from /mnt/app/package.json..."
    cp /mnt/app/package.json /app/tests/
    cd /app/tests
    npm install
    echo "✓ Dependencies installed"
fi

echo ""
echo "All files copied. Running entrypoint..."
echo "=========================================="
echo ""

# Run the entrypoint
export PAUSE_BEFORE_TESTS=false
exec /app/entrypoint.sh
