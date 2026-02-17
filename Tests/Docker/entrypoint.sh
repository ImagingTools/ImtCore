#!/bin/bash
# Docker entrypoint script for test environment
# Handles startup sequence: PostgreSQL -> Custom Apps -> Tests
#
# This script allows users to customize the startup sequence by adding
# their own initialization scripts in /app/custom-apps/

set -e

# Colors for output
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
NC='\033[0m' # No Color

echo -e "${GREEN}========================================${NC}"
echo -e "${GREEN}Starting Test Environment${NC}"
echo -e "${GREEN}========================================${NC}"

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
        # NOTE: with network_mode: host, listen_addresses='*' exposes postgres on the host network.
        # If you don't want that, change '*' to 'localhost'.
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
    # We'll detect collections + environments separately.
    if [ -d "/app/tests/API" ] && [ "$(ls -A /app/tests/API 2>/dev/null)" ]; then
        if find /app/tests/API -type f -name "*collection*.json" | grep -q .; then
            API_TESTS_FOUND=true
            echo -e "${GREEN}✓ Found Postman collections in API folder${NC}"
        fi
    fi

    if [ "$GUI_TESTS_FOUND" = true ] || [ "$API_TESTS_FOUND" = true ]; then
        EXIT_CODE=0

        if [ "$GUI_TESTS_FOUND" = true ]; then
            # Check if node_modules already exist (pre-installed in image)
            if [ -d "/modules/node_modules/playwright" ]; then
                echo -e "${GREEN}✓ Using pre-installed Playwright from /modules${NC}"
            else
                echo -e "${YELLOW}Preparing Playwright dependencies...${NC}"
                if [ -f "/app/tests/package.json" ]; then
                    if [ -f "/app/tests/package-lock.json" ]; then
                        (cd /app/tests && npm ci) || EXIT_CODE=$?
                    else
                        (cd /app/tests && npm install) || EXIT_CODE=$?
                    fi
                elif [ -f "/app/tests/GUI/package.json" ]; then
                    if [ -f "/app/tests/GUI/package-lock.json" ]; then
                        (cd /app/tests/GUI && npm ci) || EXIT_CODE=$?
                    else
                        (cd /app/tests/GUI && npm install) || EXIT_CODE=$?
                    fi
                else
                    echo -e "${RED}No /app/tests/package.json or /app/tests/GUI/package.json found. Cannot install @playwright/test.${NC}"
                    EXIT_CODE=1
                fi
            fi

            if [ "$EXIT_CODE" -eq 0 ]; then
                echo -e "${YELLOW}Running Playwright tests...${NC}"
                cd /app/tests/GUI
                
                # Use pre-installed playwright directly, not npx (avoids version conflicts)
                PLAYWRIGHT_CMD="playwright test"
                if [ "${UPDATE_SNAPSHOTS:-false}" = "true" ]; then
                    echo -e "${YELLOW}UPDATE_SNAPSHOTS=true -> updating reference screenshots${NC}"
                    PLAYWRIGHT_CMD="$PLAYWRIGHT_CMD --update-snapshots"
                fi
                
                $PLAYWRIGHT_CMD || EXIT_CODE=$?
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

            while IFS= read -r -d '' collection; do
                echo -e "${YELLOW}Running collection: $(basename "$collection")${NC}"
                # shellcheck disable=SC2086
                newman run "$collection" $ENV_OPT || EXIT_CODE=$?
            done < <(find /app/tests/API -type f -name "*collection*.json" -print0)
        fi

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