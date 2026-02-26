#!/bin/bash
# Docker entrypoint script for test environment
# Handles startup sequence: PostgreSQL -> Custom Apps -> Tests
#
# This script allows users to customize the startup sequence by adding
# their own initialization scripts in /app/startup/

set -e

# Ensure global npm modules are in PATH (newman, playwright, etc.)
export PATH="/modules/node_modules/.bin:$PATH"
export NODE_PATH="/modules/node_modules:${NODE_PATH:-}"

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
echo -e "${YELLOW}[DEBUG] NODE_PATH: $NODE_PATH${NC}"
which newman && echo -e "${GREEN}✓ newman found: $(which newman)${NC}" || echo -e "${RED}✗ newman not found${NC}"
which npx && echo -e "${GREEN}✓ npx found: $(which npx)${NC}" || echo -e "${RED}✗ npx not found${NC}"

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
            su - postgres -c "$PG_BIN_DIR/initdb -D /var/lib/postgresql/data --auth-host=scram-sha-256"
        fi

        # Ensure PostgreSQL listens on TCP (localhost at minimum)
        if ! grep -q "^listen_addresses=" /var/lib/postgresql/data/postgresql.conf 2>/dev/null; then
            su - postgres -c "echo \"listen_addresses='localhost'\" >> /var/lib/postgresql/data/postgresql.conf" || true
        fi

        # If postgres was not shut down cleanly, postmaster.pid can block startup.
        if [ -f "/var/lib/postgresql/data/postmaster.pid" ]; then
            OLD_PID="$(head -n 1 /var/lib/postgresql/data/postmaster.pid 2>/dev/null || true)"
            if [ -n "$OLD_PID" ] && kill -0 "$OLD_PID" 2>/dev/null; then
                echo -e "${YELLOW}PostgreSQL appears to be running already (pid=$OLD_PID). Skipping start.${NC}"
            else
                echo -e "${YELLOW}Found stale postmaster.pid (pid=$OLD_PID). Removing...${NC}"
                rm -f /var/lib/postgresql/data/postmaster.pid
                echo -e "${YELLOW}Starting PostgreSQL...${NC}"
                su - postgres -c "$PG_BIN_DIR/pg_ctl -D /var/lib/postgresql/data -l /var/log/postgresql/postgresql.log start"
            fi
        else
            echo -e "${YELLOW}Starting PostgreSQL...${NC}"
            su - postgres -c "$PG_BIN_DIR/pg_ctl -D /var/lib/postgresql/data -l /var/log/postgresql/postgresql.log start"
        fi

        # Wait for PostgreSQL to be ready (local socket)
        wait_for_service "PostgreSQL" "su - postgres -c 'psql -c \"SELECT 1\"'"

        # Set password for postgres user
        echo -e "${YELLOW}Setting postgres password (default)${NC}"
        su - postgres -c "psql -v ON_ERROR_STOP=1 -c \"ALTER USER postgres WITH PASSWORD '${POSTGRES_PASSWORD}';\""

        # Create test database if specified
        if [ -n "${POSTGRES_DB}" ]; then
            echo -e "${YELLOW}Creating database: ${POSTGRES_DB}${NC}"
            su - postgres -c "psql -c \"CREATE DATABASE ${POSTGRES_DB}\"" || echo "Database may already exist"
        fi
    fi
else
    echo -e "${YELLOW}Step 1: PostgreSQL startup skipped (START_POSTGRESQL not set to true)${NC}"
fi

# Step 2: Run custom application installers and startup scripts
if [ -d "/app/startup" ]; then
    echo -e "${YELLOW}Step 2: Running custom application scripts...${NC}"
    echo -e "${YELLOW}Startup dir: /app/startup${NC}"

    echo -e "${YELLOW}Listing /app/startup:${NC}"
    ls -la /app/startup || true

    # Export path environment variables for startup scripts
    export APP_DIR="/app"
    export STARTUP_DIR="/app/startup"
    export RESOURCES_DIR="/app/resources"
    export TESTS_DIR="/app/tests"

    echo -e "${YELLOW}Environment variables set for startup scripts:${NC}"
    echo -e "${YELLOW}  APP_DIR=$APP_DIR${NC}"
    echo -e "${YELLOW}  STARTUP_DIR=$STARTUP_DIR${NC}"
    echo -e "${YELLOW}  RESOURCES_DIR=$RESOURCES_DIR${NC}"
    echo -e "${YELLOW}  TESTS_DIR=$TESTS_DIR${NC}"

    # Normalize scripts copied from Windows to avoid UTF-8 BOM / CRLF issues
    echo -e "${YELLOW}Normalizing .sh scripts (BOM/CRLF) and setting +x...${NC}"
    shopt -s nullglob
    STARTUP_SCRIPTS=(/app/startup/*.sh)
    shopt -u nullglob

    if [ ${#STARTUP_SCRIPTS[@]} -eq 0 ]; then
        echo -e "${YELLOW}No startup scripts found: /app/startup/*.sh${NC}"
    else
        echo -e "${GREEN}Found ${#STARTUP_SCRIPTS[@]} startup script(s):${NC}"
        for s in "${STARTUP_SCRIPTS[@]}"; do
            echo -e "${GREEN}  - $(basename "$s")${NC}"
        done

        for f in "${STARTUP_SCRIPTS[@]}"; do
            [ -f "$f" ] || continue

            echo -e "${YELLOW}Preparing script: $(basename "$f")${NC}"

            # Show first line to quickly spot wrong shebang/BOM
            head -n 1 "$f" 2>/dev/null | sed 's/\r$//' || true

            # Remove UTF-8 BOM if present (EF BB BF)
            sed -i '1s/^\xEF\xBB\xBF//' "$f" || true

            # Convert CRLF -> LF
            sed -i 's/\r$//' "$f" || true

            chmod +x "$f" || true
        done

        echo -e "${YELLOW}Executing startup scripts in lexicographic order...${NC}"
        for startup_script in "${STARTUP_SCRIPTS[@]}"; do
            echo -e "${YELLOW}Running startup script: $(basename "$startup_script")${NC}"
            /bin/bash "$startup_script"
            echo -e "${GREEN}Finished: $(basename "$startup_script") (exit=$?)${NC}"
        done
    fi

    echo -e "${GREEN}✓ Custom applications initialized${NC}"
else
    echo -e "${YELLOW}Step 2: No custom applications to start (/app/startup not found)${NC}"
fi

# Step 3: Run tests or custom command
echo -e "${YELLOW}Step 3: Starting tests...${NC}"
echo -e "${GREEN}========================================${NC}"

cd /app/tests

# Ensure test-results directory exists
mkdir -p /app/tests/test-results

if [ "${PAUSE_BEFORE_TESTS:-false}" = "true" ]; then
    echo -e "${YELLOW}PAUSE_BEFORE_TESTS=true -> pausing before running tests.${NC}"
    echo -e "${YELLOW}Attach with: docker compose exec tests sh${NC}"
    sleep infinity
fi

# If a command is provided, run it; otherwise, auto-detect tests
if [ "$#" -eq 0 ] || [ "$1" = "echo" ]; then
    echo -e "${YELLOW}Auto-detecting tests...${NC}"

    GUI_TESTS_FOUND=false
    API_TESTS_FOUND=false

    # Check for GUI tests (Playwright) - recursive, supports *.spec.* and *.test.*
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

    if [ "$GUI_TESTS_FOUND" = true ] || [ "$API_TESTS_FOUND" = true ]; then
        EXIT_CODE=0

        if [ "$GUI_TESTS_FOUND" = true ]; then
            echo -e "${YELLOW}Preparing Playwright dependencies...${NC}"
            
            # Check if node_modules already exist (pre-installed in image)
            if [ -d "/modules/node_modules/@playwright/test" ]; then
                echo -e "${GREEN}✓ Using pre-installed Playwright from /modules${NC}"
            else
                echo -e "${YELLOW}Playwright not found in /modules, checking for local package.json...${NC}"
                
                # Find package.json location
                PACKAGE_DIR=""
                if [ -f "/app/tests/GUI/package.json" ]; then
                    PACKAGE_DIR="/app/tests/GUI"
                elif [ -f "/app/tests/package.json" ]; then
                    PACKAGE_DIR="/app/tests"
                fi
                
                if [ -z "$PACKAGE_DIR" ]; then
                    echo -e "${RED}✗ No package.json found in /app/tests or /app/tests/GUI. Cannot install dependencies.${NC}"
                    EXIT_CODE=1
                else
                    echo -e "${YELLOW}Installing dependencies from $PACKAGE_DIR...${NC}"
                    cd "$PACKAGE_DIR"
                    if [ -f "package-lock.json" ]; then
                        npm ci || EXIT_CODE=$?
                    else
                        npm install || EXIT_CODE=$?
                    fi
                    
                    if [ "$EXIT_CODE" -ne 0 ]; then
                        echo -e "${RED}✗ npm install failed${NC}"
                    else
                        echo -e "${GREEN}✓ Dependencies installed successfully${NC}"
                        # Update NODE_PATH to prioritize local node_modules
                        export NODE_PATH="$PACKAGE_DIR/node_modules:${NODE_PATH:-}"
                        echo -e "${GREEN}✓ Local node_modules added to NODE_PATH${NC}"
                    fi
                fi
            fi

            if [ "$EXIT_CODE" -eq 0 ]; then
                echo -e "${YELLOW}Running Playwright tests...${NC}"
                
                # Add GUI directory to NODE_PATH for utils.js and other test utilities
                # (this adds to any existing NODE_PATH set during dependency installation)
                export NODE_PATH="/app/tests/GUI:${NODE_PATH:-}"
                echo -e "${GREEN}✓ Final NODE_PATH: $NODE_PATH${NC}"
                
                # Build playwright command with optional --update-snapshots flag
                PLAYWRIGHT_CMD="npx playwright test --output=/app/tests/test-results/playwright-output"
                if [ "${UPDATE_SNAPSHOTS:-false}" = "true" ]; then
                    echo -e "${YELLOW}UPDATE_SNAPSHOTS=true -> updating reference screenshots${NC}"
                    PLAYWRIGHT_CMD="$PLAYWRIGHT_CMD --update-snapshots"
                fi
                
                (cd /app/tests/GUI && $PLAYWRIGHT_CMD) || EXIT_CODE=$?
                
                # Copy Playwright report to test-results
                if [ -d "/app/tests/GUI/playwright-report" ]; then
                    echo -e "${YELLOW}Copying Playwright report to test-results...${NC}"
                    cp -r /app/tests/GUI/playwright-report /app/tests/test-results/
                fi
                
                # Copy test-results from GUI folder if exists
                if [ -d "/app/tests/GUI/test-results" ]; then
                    echo -e "${YELLOW}Copying Playwright test-results...${NC}"
                    cp -r /app/tests/GUI/test-results/* /app/tests/test-results/ 2>/dev/null || true
                fi
            fi
        fi

        if [ "$API_TESTS_FOUND" = true ]; then
            echo -e "${YELLOW}Running Postman tests...${NC}"

            # Use first environment file if exists (optional)
            ENV_OPT=""
            ENV_FILE="$(find /app/tests/API -type f -name "*environment*.json" | head -n 1 || true)"
            if [ -n "$ENV_FILE" ]; then
                ENV_OPT="-e $ENV_FILE"
                echo -e "${YELLOW}Using environment: $(basename "$ENV_FILE")${NC}"
            fi

            # Run newman collections with reporters
            while IFS= read -r -d '' collection; do
                COLLECTION_NAME="$(basename "$collection" .json)"
                echo -e "${YELLOW}Running collection: ${COLLECTION_NAME}${NC}"
                # shellcheck disable=SC2086
                newman run "$collection" $ENV_OPT \
                    --reporters cli,json,htmlextra \
                    --reporter-json-export "/app/tests/test-results/newman-${COLLECTION_NAME}.json" \
                    --reporter-htmlextra-export "/app/tests/test-results/newman-${COLLECTION_NAME}.html" \
                    || EXIT_CODE=$?
            done < <(find /app/tests/API -type f -name "*collection*.json" -print0)
        fi

        echo -e "${GREEN}========================================${NC}"
        echo -e "${GREEN}Test results saved to: /app/tests/test-results/${NC}"
        echo -e "${YELLOW}Contents:${NC}"
        ls -la /app/tests/test-results/ || true
        echo -e "${GREEN}========================================${NC}"

        exit $EXIT_CODE
    else
        echo -e "${YELLOW}No tests found in GUI or API folders${NC}"
        echo -e "${YELLOW}To run tests:${NC}"
        echo -e "${YELLOW}  - Copy Playwright tests to /app/tests/GUI/${NC}"
        echo -e "${YELLOW}  - Copy Postman collections to /app/tests/API/${NC}"
        exit 0
    fi
else
    exec "$@"
fi