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

# Determine ImtCore directory
if [ -z "$IMTCOREDIR" ]; then
    # If IMTCOREDIR is not set, assume ImtCore is at the same level as the application
    SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
    # Go up to application root (assuming script is in App/Tests/)
    APP_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
    # Assume ImtCore is at same level as application
    IMTCOREDIR="$(cd "$APP_ROOT/../ImtCore" 2>/dev/null && pwd)"
    
    if [ -z "$IMTCOREDIR" ]; then
        echo "ERROR: IMTCOREDIR environment variable is not set and ImtCore not found at expected location."
        echo "Please either:"
        echo "  1. Set IMTCOREDIR environment variable: export IMTCOREDIR=/path/to/ImtCore"
        echo "  2. Place ImtCore at the same level as your application directory"
        exit 1
    fi
fi

# Validate ImtCore path
if [ ! -f "$IMTCOREDIR/Tests/Docker/Scripts/run-tests-linux-core.sh" ]; then
    echo "ERROR: Cannot find core script at: $IMTCOREDIR/Tests/Docker/Scripts/run-tests-linux-core.sh"
    echo "Please ensure IMTCOREDIR points to a valid ImtCore directory."
    exit 1
fi

bash "$IMTCOREDIR/Tests/Docker/Scripts/run-tests-linux-core.sh"
exit $?
