#!/bin/bash
# Wrapper script - calls the core script with default/example configuration
# For production use in application repos, copy run-tests-linux-template.sh instead

# Set defaults if not already set
export IMAGE_NAME="${IMAGE_NAME:-imtcore-tests:linux}"
export START_POSTGRESQL="${START_POSTGRESQL:-true}"

# Call the core script from the same directory
SCRIPT_DIR="$(dirname "$0")"
bash "$SCRIPT_DIR/run-tests-linux-core.sh"
exit $?
