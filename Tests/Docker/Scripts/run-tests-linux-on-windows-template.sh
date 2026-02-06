#!/bin/bash
# Simple configuration file for running Docker tests (Linux containers on Windows)
# Copy this file to your application's Tests directory and configure the variables below

# ==========================================
# CONFIGURATION
# ==========================================

# Application URL (use host.docker.internal to access Windows host)
export BASE_URL="http://host.docker.internal:7776"

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
if [ ! -f "$IMTCOREDIR/Tests/Docker/Scripts/run-tests-linux-on-windows-core.sh" ]; then
    echo "ERROR: Cannot find core script at: $IMTCOREDIR/Tests/Docker/Scripts/run-tests-linux-on-windows-core.sh"
    echo "Please ensure IMTCOREDIR points to a valid ImtCore directory."
    exit 1
fi

bash "$IMTCOREDIR/Tests/Docker/Scripts/run-tests-linux-on-windows-core.sh"
exit $?
