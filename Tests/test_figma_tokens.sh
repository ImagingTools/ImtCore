#!/bin/bash

# Test script for Figma token processor
# This script tests the Figma design token conversion

set -e

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
REPO_ROOT="${SCRIPT_DIR}/.."
TEST_OUTPUT_DIR="${SCRIPT_DIR}/test_output"

echo "=== Figma Design Token Processor Test ==="
echo ""

# Create test output directory
mkdir -p "${TEST_OUTPUT_DIR}"

# Path to example Figma tokens file
FIGMA_TOKENS_FILE="${REPO_ROOT}/Include/imtstyle/Resources/figma-tokens-example.json"

if [ ! -f "${FIGMA_TOKENS_FILE}" ]; then
    echo "ERROR: Figma tokens example file not found: ${FIGMA_TOKENS_FILE}"
    exit 1
fi

echo "✓ Found Figma tokens example file"
echo ""

# Validate JSON format
if command -v python3 &> /dev/null; then
    echo "Validating JSON format..."
    if python3 << EOF
import json
try:
    with open('${FIGMA_TOKENS_FILE}', 'r') as f:
        json.load(f)
except Exception as e:
    print(f"Error: {e}")
    exit(1)
EOF
    then
        echo "✓ JSON is valid"
    else
        echo "✗ JSON is invalid"
        exit 1
    fi
else
    echo "⚠ Python3 not available, skipping JSON validation"
fi
echo ""

# Display token structure
echo "Token structure:"
if command -v python3 &> /dev/null; then
    python3 << EOF
import json

with open('${FIGMA_TOKENS_FILE}', 'r') as f:
    tokens = json.load(f)
    
print("  Sections found:")
for key in tokens.keys():
    print(f"    - {key}")
    if isinstance(tokens[key], dict):
        print(f"      Items: {len(tokens[key])}")
EOF
fi
echo ""

# Check if DesignTokenCreator executable exists
DESIGN_TOKEN_CREATOR="${REPO_ROOT}/Build/bin/DesignTokenCreator"
if [ ! -f "${DESIGN_TOKEN_CREATOR}" ]; then
    echo "⚠ DesignTokenCreator executable not found at: ${DESIGN_TOKEN_CREATOR}"
    echo "  Build the project first to run the full test"
    echo ""
    echo "To build:"
    echo "  cd Build && cmake .. && make"
    echo ""
    echo "=== Test completed with warnings ==="
    exit 0
fi

echo "✓ Found DesignTokenCreator executable"
echo ""

# Run the Figma token processor
echo "Running Figma token processor..."
if "${DESIGN_TOKEN_CREATOR}" \
    --figma "${FIGMA_TOKENS_FILE}" \
    -OD "${TEST_OUTPUT_DIR}" \
    -P "TestApp"
then
    echo "✓ Processor executed successfully"
else
    echo "✗ Processor failed"
    exit 1
fi
echo ""

# Check output file
OUTPUT_THEME_FILE="${TEST_OUTPUT_DIR}/TestApp_figma.theme"
if [ -f "${OUTPUT_THEME_FILE}" ]; then
    echo "✓ Output theme file created: ${OUTPUT_THEME_FILE}"
    echo ""
    
    # Validate output JSON
    if command -v python3 &> /dev/null; then
        echo "Validating output theme file..."
        python3 << EOF
import json

with open('${OUTPUT_THEME_FILE}', 'r') as f:
    theme = json.load(f)
    
print("✓ Output theme is valid JSON")
print("")
print("Theme structure:")
for key in theme.keys():
    print(f"  - {key}")
    if isinstance(theme[key], dict):
        if key == "ColorPalette":
            print(f"    Colors: {len(theme[key])}")
        elif key == "Style":
            style = theme[key]
            for style_key in style.keys():
                print(f"    - {style_key}")
EOF
    fi
    echo ""
    echo "Output preview (first 30 lines):"
    head -n 30 "${OUTPUT_THEME_FILE}"
else
    echo "✗ Output theme file not created"
    exit 1
fi

echo ""
echo "=== Test completed successfully ==="
echo "Output saved to: ${TEST_OUTPUT_DIR}"
