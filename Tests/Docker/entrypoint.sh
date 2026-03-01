#!/bin/bash
# Docker entrypoint script for test environment
# Handles startup sequence: PostgreSQL -> Custom Apps -> Tests
#
# This script allows users to customize the startup sequence by adding
# their own initialization scripts in /app/startup/

set -e

# Use tools installed in /modules, but do NOT set NODE_PATH to /modules/node_modules
# NODE_PATH can cause Node to resolve dependencies from unexpected locations,
# leading to multiple @playwright/test copies and "test.beforeEach()" errors.
export PATH="/modules/node_modules/.bin:$PATH"
unset NODE_PATH

# Colors for output
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
NC='\033[0m' # No Color

echo -e "${GREEN}========================================${NC}"
echo -e "${GREEN}Starting Test Environment${NC}"
echo -e "${GREEN}========================================${NC}"

# Debug: show PATH and verify tools are available
echo -e "${YELLOW}[DEBUG] PATH: $PATH${NC}"
echo -e "${YELLOW}[DEBUG] NODE_PATH: ${NODE_PATH:-<unset>}${NC}"
which newman && echo -e "${GREEN}✓ newman found: $(which newman)${NC}" || echo -e "${RED}✗ newman not found${NC}"
which npx && echo -e "${GREEN}✓ npx found: $(which npx)${NC}" || echo -e "${RED}✗ npx not found${NC}"
which playwright && echo -e "${GREEN}✓ playwright found: $(which playwright)${NC}" || echo -e "${RED}✗ playwright not found${NC}"

# Function to wait for a service to be ready
wait_for_service() {
    local service_name=$1
    local check_command=$2
    local max_attempts=${3:-30}
    local attempt=1

    echo -e "${YELLOW}Waiting for ${service_name} to be ready...${NC}"

    while [ $attempt -le $max_attempts ]; do
        if eval "$check_command" > /dev/null 2>&1; then
            echo -e "${GREEN}✓ ${service_name} is ready${NC}"
            return 0
        fi
        echo -e "  Attempt $attempt/$max_attempts..."
        sleep 2
        ((attempt++))
    done

    echo -e "${RED}✗ ${service_name} failed to start${NC}"
    return 1
}

# Step 1: Start PostgreSQL if needed
if [ "${START_POSTGRESQL:-false}" = "true" ]; then
    echo -e "${YELLOW}Step 1: Starting PostgreSQL...${NC}"

    # Default password for postgres user (can be overridden)
    POSTGRES_PASSWORD="${POSTGRES_PASSWORD:-root}"

    # Find PostgreSQL binary directory
    PG_BIN_DIR=$(find /usr/lib/postgresql -name bin -type d 2>/dev/null | head -1)

    if [ -z "$PG_BIN_DIR" ]; then
        echo -e "${RED}✗ PostgreSQL not found in /usr/lib/postgresql${NC}"
        echo -e "${YELLOW}Continuing without PostgreSQL...${NC}"
    else
        # Create log directory
        mkdir -p /var/log/postgresql
        chown -R postgres:postgres /var/log/postgresql

        # Ensure data dir exists and has correct ownership
        mkdir -p /var/lib/postgresql/data
        chown -R postgres:postgres /var/lib/postgresql

        # Initialize PostgreSQL only if not initialized (PG_VERSION is the canonical marker)
        if [ ! -f "/var/lib/postgresql/data/PG_VERSION" ]; then
            echo -e "${YELLOW}Initializing PostgreSQL data directory...${NC}"
            su - postgres -c "$PG_BIN_DIR/initdb -D /var/lib/postgresql/data"
        fi

        echo -e "${YELLOW}Starting PostgreSQL with max_connections=300...${NC}"
        # Added -o '-c max_connections=300' here
        su - postgres -c "$PG_BIN_DIR/pg_ctl -D /var/lib/postgresql/data -l /var/log/postgresql/postgresql.log -o '-c max_connections=300' start"

        wait_for_service "PostgreSQL" "su - postgres -c \"$PG_BIN_DIR/psql -c 'SELECT 1'\""

        echo -e "${YELLOW}Setting postgres password...${NC}"
        su - postgres -c "$PG_BIN_DIR/psql -c \"ALTER USER postgres WITH PASSWORD '${POSTGRES_PASSWORD}';\""

        # --- Проверка пароля PostgreSQL ---
        echo -e "${YELLOW}Checking PostgreSQL password...${NC}"
        if ! su - postgres -c "PGPASSWORD=root $PG_BIN_DIR/psql -U postgres -h 127.0.0.1 -d postgres -c 'SELECT 1;' > /dev/null 2>&1"; then
            echo -e "${RED}✗ PostgreSQL password check failed!${NC}"
            echo -e "${RED}Ensure POSTGRES_PASSWORD is set correctly and the server is running.${NC}"
            exit 1
        else
            echo -e "${GREEN}✓ PostgreSQL password 'root' is valid${NC}"
        fi

        # NOTE:
        # Removed database creation logic because it caused interactive password prompts
        # and "su: Authentication failure" in some environments.
        if [ -n "${POSTGRES_DB:-}" ]; then
            echo -e "${YELLOW}POSTGRES_DB is set to '${POSTGRES_DB}', but automatic CREATE DATABASE is disabled in entrypoint.${NC}"
            echo -e "${YELLOW}If you need the database to exist, create it in your own startup script under /app/startup/.${NC}"
        fi

        echo -e "${GREEN}✓ PostgreSQL initialized${NC}"
    fi
else
    echo -e "${YELLOW}Step 1: PostgreSQL startup skipped${NC}"
fi

# Step 2: Run custom application installers and startup scripts
if [ -d "/app/startup" ] && [ "$(ls -A /app/startup 2>/dev/null)" ]; then
    echo -e "${YELLOW}Step 2: Running custom application scripts...${NC}"
    echo -e "${YELLOW}Startup dir: /app/startup${NC}"
    ls -la /app/startup || true

    export APP_DIR=/app
    export STARTUP_DIR=/app/startup
    export RESOURCES_DIR=/app/resources
    export TESTS_DIR=/app/tests

    for script in /app/startup/*; do
        if [ -f "$script" ]; then
            echo -e "${YELLOW}Running startup script: $(basename "$script")${NC}"
            case "$script" in
                *.sh)  chmod +x "$script" 2>/dev/null || true; "$script" ;;
                *.bash) chmod +x "$script" 2>/dev/null || true; bash "$script" ;;
                *.py)  python3 "$script" ;;
                *)     echo -e "${YELLOW}Skipping unknown script type: $script${NC}" ;;
            esac
        fi
    done

    echo -e "${GREEN}✓ Custom applications initialized${NC}"
else
    echo -e "${YELLOW}Step 2: No custom applications to start${NC}"
fi

# Step 3: Run tests
echo -e "${YELLOW}Step 3: Starting tests...${NC}"
echo -e "${GREEN}========================================${NC}"

if [ "${PAUSE_BEFORE_TESTS:-false}" = "true" ]; then
    echo -e "${YELLOW}PAUSE_BEFORE_TESTS=true - pausing before running tests.${NC}"
    sleep 3600
fi

echo -e "${YELLOW}Auto-detecting tests...${NC}"

GUI_TESTS_FOUND=false
API_TESTS_FOUND=false

# Check for GUI tests (Playwright)
if [ -d "/app/tests/GUI" ] && [ "$(ls -A /app/tests/GUI 2>/dev/null)" ]; then
    if find /app/tests/GUI -type f \( -name "*.spec.js" -o -name "*.spec.ts" -o -name "*.test.js" -o -name "*.test.ts" \) | grep -q .; then
        GUI_TESTS_FOUND=true
        echo -e "${GREEN}✓ Found Playwright tests in GUI folder${NC}"
    fi
fi

# Check for API tests (Postman/Newman)
if [ -d "/app/tests/API" ] && [ "$(ls -A /app/tests/API 2>/dev/null)" ]; then
    if find /app/tests/API -type f -name "*collection*.json" | grep -q .; then
        API_TESTS_FOUND=true
        echo -e "${GREEN}✓ Found Postman collections in API folder${NC}"
    fi
fi

EXIT_CODE=0

if [ "$GUI_TESTS_FOUND" = true ]; then
    echo -e "${YELLOW}Preparing Playwright dependencies...${NC}"

    if [ ! -d "/modules/node_modules/@playwright/test" ]; then
        echo -e "${RED}✗ @playwright/test not found in /modules/node_modules${NC}"
        echo -e "${RED}✗ The Docker image may not have been built correctly.${NC}"
        EXIT_CODE=1
    fi

    # Detect accidental local installations that can cause duplicate versions
    if find /app -path "*/node_modules/@playwright/test" -type d 2>/dev/null | grep -q .; then
        echo -e "${RED}✗ Found @playwright/test under /app/node_modules (duplicate install).${NC}"
        echo -e "${RED}✗ Remove local node_modules from mounted tests to avoid version conflicts.${NC}"
        find /app -path "*/node_modules/@playwright/test" -type d 2>/dev/null || true
        EXIT_CODE=1
    fi

    if [ "$EXIT_CODE" -eq 0 ]; then
        echo -e "${YELLOW}Running Playwright tests...${NC}"

        # Make /modules-installed dependencies resolvable from the mounted test folder
        # so that /app/tests/GUI/playwright.config.js can `require('@playwright/test')`.
        if [ ! -e "/app/tests/GUI/node_modules" ]; then
            ln -s /modules/node_modules /app/tests/GUI/node_modules
            echo -e "${GREEN}✓ Linked /app/tests/GUI/node_modules -> /modules/node_modules${NC}"
        fi

        # Only add GUI utilities path; do NOT add /modules to NODE_PATH
        export NODE_PATH="/app/tests/GUI"
        echo -e "${GREEN}✓ NODE_PATH set to: $NODE_PATH${NC}"

        PLAYWRIGHT_BIN="/modules/node_modules/.bin/playwright"
        PLAYWRIGHT_CMD="$PLAYWRIGHT_BIN test --output=/app/tests/test-results/playwright-output"

        (cd /app/tests/GUI && $PLAYWRIGHT_CMD) || EXIT_CODE=$?

        if [ -d "/app/tests/GUI/playwright-report" ]; then
            echo -e "${YELLOW}Copying Playwright report to test-results...${NC}"
            cp -r /app/tests/GUI/playwright-report /app/tests/test-results/
        fi

        if [ -d "/app/tests/GUI/test-results" ]; then
            echo -e "${YELLOW}Copying Playwright test-results...${NC}"
            cp -r /app/tests/GUI/test-results/* /app/tests/test-results/ 2>/dev/null || true
        fi
    fi
fi

if [ "$API_TESTS_FOUND" = true ]; then
    echo -e "${YELLOW}Running Postman tests...${NC}"

    ENV_OPT=""
    ENV_FILE=$(find /app/tests/API -type f -iname "*environment*.json" 2>/dev/null | head -1 || true)
    if [ -n "$ENV_FILE" ]; then
        ENV_OPT="-e $ENV_FILE"
        echo -e "${GREEN}✓ Using environment: $ENV_FILE${NC}"
    fi

    for collection in $(find /app/tests/API -type f -iname "*collection*.json" 2>/dev/null); do
        echo -e "${YELLOW}Running collection: $(basename "$collection")${NC}"
        newman run "$collection" $ENV_OPT || EXIT_CODE=$?
    done
fi

if [ "$GUI_TESTS_FOUND" = false ] && [ "$API_TESTS_FOUND" = false ]; then
    echo -e "${YELLOW}No tests found in GUI or API folders${NC}"
    EXIT_CODE=0
fi

exit $EXIT_CODE