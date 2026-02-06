#!/bin/bash
# Template wrapper script for running tests in application repos
# Copy this file to your application repo's Tests directory and configure it for your needs

# ==========================================
# CONFIGURATION - MODIFY THESE FOR YOUR APP
# ==========================================

# Your application URL (use host.docker.internal to access Windows host)
export BASE_URL="http://host.docker.internal:7776"

# PostgreSQL settings (set START_POSTGRESQL=false if you don't need PostgreSQL)
export START_POSTGRESQL="true"
export POSTGRES_DB="myapp_test"
export DATABASE_URL="postgresql://postgres:postgres@localhost:5432/myapp_test"

# Test user credentials
export TEST_USERNAME="test@example.com"
export TEST_PASSWORD="testpassword"

# Docker image name (usually don't need to change this)
export IMAGE_NAME="imtcore-tests:linux"

# ==========================================
# CORE LOGIC - DO NOT MODIFY BELOW THIS LINE
# ==========================================

# Find ImtCore path (adjust this path to point to your ImtCore location)
IMTCORE_PATH="$(dirname "$0")/../../../../ImtCore"

if [ ! -f "$IMTCORE_PATH/Tests/Docker/Scripts/run-tests-linux-on-windows-core.sh" ]; then
    echo "ERROR: Cannot find ImtCore at $IMTCORE_PATH"
    echo "Please update IMTCORE_PATH in this script to point to your ImtCore directory"
    exit 1
fi

# Call the core script from ImtCore
bash "$IMTCORE_PATH/Tests/Docker/Scripts/run-tests-linux-on-windows-core.sh"
exit $?
