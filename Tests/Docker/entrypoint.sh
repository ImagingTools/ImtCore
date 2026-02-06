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
if [ -d "/app/custom-apps" ]; then
    echo -e "${YELLOW}Step 2: Running custom application scripts...${NC}"

    # Run startup scripts in order (01-*, 02-*, etc.)
    if [ -d "/app/custom-apps/startup" ]; then
        # Normalize scripts copied from Windows to avoid UTF-8 BOM / CRLF issues
        for f in /app/custom-apps/startup/*.sh; do
            [ -f "$f" ] || continue

            # Remove UTF-8 BOM if present (EF BB BF)
            sed -i '1s/^\xEF\xBB\xBF//' "$f" || true

            # Convert CRLF -> LF
            sed -i 's/\r$//' "$f" || true

            chmod +x "$f" || true
        done

        shopt -s nullglob
        for startup_script in /app/custom-apps/startup/*.sh; do
            if [ -f "$startup_script" ] && [ -x "$startup_script" ]; then
                echo -e "${YELLOW}Running startup script: $(basename "$startup_script")${NC}"
                /bin/bash "$startup_script"
            fi
        done
        shopt -u nullglob
    fi

    echo -e "${GREEN}✓ Custom applications initialized${NC}"
else
    echo -e "${YELLOW}Step 2: No custom applications to start (/app/custom-apps not found)${NC}"
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

# Check if we should run tests automatically based on folder contents
# If a command is provided, run it; otherwise, auto-detect tests
if [ "$#" -eq 0 ] || [ "$1" = "echo" ]; then
    echo -e "${YELLOW}Auto-detecting tests...${NC}"

    GUI_TESTS_FOUND=false
    API_TESTS_FOUND=false

    # Check for GUI tests (Playwright)
    if [ -d "/app/tests/GUI" ] && [ "$(ls -A /app/tests/GUI 2>/dev/null)" ]; then
        if ls /app/tests/GUI/*.spec.js 1> /dev/null 2>&1 || ls /app/tests/GUI/*.spec.ts 1> /dev/null 2>&1; then
            GUI_TESTS_FOUND=true
            echo -e "${GREEN}✓ Found Playwright tests in GUI folder${NC}"
        fi
    fi

    # Check for API tests (Postman/Newman)
    if [ -d "/app/tests/API" ] && [ "$(ls -A /app/tests/API 2>/dev/null)" ]; then
        if ls /app/tests/API/*.json 1> /dev/null 2>&1; then
            API_TESTS_FOUND=true
            echo -e "${GREEN}✓ Found Postman collections in API folder${NC}"
        fi
    fi

    # Run tests based on what was found
    if [ "$GUI_TESTS_FOUND" = true ] || [ "$API_TESTS_FOUND" = true ]; then
        EXIT_CODE=0

        if [ "$GUI_TESTS_FOUND" = true ]; then
            echo -e "${YELLOW}Running Playwright tests...${NC}"
            npx playwright test GUI || EXIT_CODE=$?
        fi

        if [ "$API_TESTS_FOUND" = true ]; then
            echo -e "${YELLOW}Running Postman tests...${NC}"
            for collection in /app/tests/API/*.json; do
                if [ -f "$collection" ]; then
                    echo -e "${YELLOW}Running collection: $(basename "$collection")${NC}"
                    newman run "$collection" || EXIT_CODE=$?
                fi
            done
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
    # Run the provided command
    exec "$@"
fi