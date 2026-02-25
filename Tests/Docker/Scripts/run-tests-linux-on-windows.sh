#!/bin/bash
# Wrapper script - calls the core script with default/example configuration
# For production use in application repos, copy run-tests-linux-on-windows-template.sh instead

# Set defaults if not already set
export BASE_URL="${BASE_URL:-http://host.docker.internal:8080}"
export POSTGRES_DB="${POSTGRES_DB:-test_db}"
export POSTGRES_PASSWORD="${POSTGRES_PASSWORD:-root}"

# Call the core script from the same directory
SCRIPT_DIR="$(dirname "$0")"
bash "$SCRIPT_DIR/run-tests-linux-on-windows-core.sh"
exit $?