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

# Auto-discover ImtCore (looks in parent directories)
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
IMTCORE_PATH="$SCRIPT_DIR/../.."

if [ ! -f "$IMTCORE_PATH/Tests/Docker/Scripts/run-tests-linux-on-windows-core.sh" ]; then
    IMTCORE_PATH="$SCRIPT_DIR/../../.."
fi

if [ ! -f "$IMTCORE_PATH/Tests/Docker/Scripts/run-tests-linux-on-windows-core.sh" ]; then
    IMTCORE_PATH="$SCRIPT_DIR/../../../.."
fi

if [ ! -f "$IMTCORE_PATH/Tests/Docker/Scripts/run-tests-linux-on-windows-core.sh" ]; then
    echo "ERROR: Cannot find ImtCore. Please ensure ImtCore is in a parent directory."
    exit 1
fi

bash "$IMTCORE_PATH/Tests/Docker/Scripts/run-tests-linux-on-windows-core.sh"
exit $?
