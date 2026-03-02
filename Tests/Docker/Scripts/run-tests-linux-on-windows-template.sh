#!/bin/bash
# Simple configuration file for running Docker tests (Linux containers on Windows)
# Copy this file to your application's Tests directory and configure the variables below

# ==========================================
# CONFIGURATION
# ==========================================

# Application URL (use host.docker.internal to access Windows host)
export BASE_URL="http://host.docker.internal:7776"

# PostgreSQL password (optional, default: "root" if not set)
# Uncomment and set if you need a different password:
# export POSTGRES_PASSWORD="your_password"

# Test credentials - supports multiple users
# Format: "username1:password1,username2:password2,username3:password3"
# Or simple single user (backward compatible):
export TEST_USERS="test@example.com:testpassword"
# Examples:
# Single user: export TEST_USERS="admin@example.com:admin123"
# Multiple users: export TEST_USERS="admin@example.com:admin123,user@example.com:user123,viewer@example.com:viewer123"

# Update Playwright reference screenshots (optional, default: false)
# Set to "true" to update all reference screenshots (visual regression baselines)
# Uncomment to enable:
# export UPDATE_SNAPSHOTS="true"

# ==========================================
# DO NOT MODIFY BELOW THIS LINE
# ==========================================

# Determine ImtCore directory
if [ -z "$IMTCOREDIR" ]; then
    # If IMTCOREDIR is not set, assume ImtCore is at ../../ImtCore
    SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
    IMTCOREDIR="$(cd "$SCRIPT_DIR/../../ImtCore" 2>/dev/null && pwd)"
fi

if [ -z "$IMTCOREDIR" ]; then
    echo "ERROR: IMTCOREDIR environment variable is not set and ImtCore not found at expected location."
    echo "Please either:"
    echo "  1. Set IMTCOREDIR environment variable: export IMTCOREDIR=/path/to/ImtCore"
    echo "  2. Place ImtCore at ../../ImtCore relative to this script"
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
