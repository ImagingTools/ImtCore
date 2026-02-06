#!/bin/bash
# Simple configuration file for running Docker tests (Linux containers on Linux)
# Copy this file to your application's Tests directory and configure the variables below

# ==========================================
# CONFIGURATION
# ==========================================

# Application URL (use localhost since we're using --network host)
export BASE_URL="http://localhost:8080"

# PostgreSQL database name
export POSTGRES_DB="myapp_test"

# PostgreSQL password (default: root)
export POSTGRES_PASSWORD="root"

# Test credentials
export TEST_USERNAME="test@example.com"
export TEST_PASSWORD="testpassword"

# ==========================================
# DO NOT MODIFY BELOW THIS LINE
# ==========================================

# Check if IMTCOREDIR environment variable is set
if [ -z "$IMTCOREDIR" ]; then
    echo "ERROR: IMTCOREDIR environment variable is not set."
    echo "Please set it to the path of your ImtCore directory:"
    echo "  export IMTCOREDIR=/path/to/ImtCore"
    exit 1
fi

# Validate ImtCore path
if [ ! -f "$IMTCOREDIR/Tests/Docker/Scripts/run-tests-linux-core.sh" ]; then
    echo "ERROR: Cannot find core script at: $IMTCOREDIR/Tests/Docker/Scripts/run-tests-linux-core.sh"
    echo "Please ensure IMTCOREDIR points to a valid ImtCore directory."
    exit 1
fi

bash "$IMTCOREDIR/Tests/Docker/Scripts/run-tests-linux-core.sh"
exit $?
