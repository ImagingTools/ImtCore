#!/usr/bin/env bash
# SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#
# JQML v2 — standalone compilation helper (Linux / macOS)
#
# Runs the full QML-to-JavaScript pipeline:
#   1. preparesources.py   — copies QML directories into the build tree
#   2. compiler/compiler.js — compiles the gathered sources to jqml.full.js
#
# Usage:
#   compile.sh <build-dir> [<src1> <dst1> ...]
#
# Arguments:
#   build-dir   Root of the web build directory.
#               The JS bundle is written to <build-dir>/src/jqml.full.js.
#   srcN dstN   Source / destination pairs forwarded to preparesources.py.
#
# Environment variables:
#   NODE_EXE    Override Node.js executable (default: node)
#   PYTHONEXE   Override Python executable  (default: python3)
#   JQML_VERBOSE=1  Enable verbose output from the compiler
#
# Example:
#   compile.sh /build/myapp/web \
#       /opt/ImtCore/Qml/imtgui     /build/myapp/web/src/imtgui \
#       /opt/ImtCore/Qml/imtcontrols /build/myapp/web/src/imtcontrols

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
COMPILER="${SCRIPT_DIR}/compiler/compiler.js"
PREPARE="${SCRIPT_DIR}/preparesources.py"

NODE="${NODE_EXE:-node}"
PYTHON="${PYTHONEXE:-python3}"

# ── Validate arguments ────────────────────────────────────────────────────────

if [[ $# -lt 1 ]]; then
    echo "Usage: $(basename "$0") <build-dir> [<src1> <dst1> ...]" >&2
    exit 2
fi

BUILD_DIR="$1"
shift

# ── Prepare output directories ────────────────────────────────────────────────

mkdir -p "${BUILD_DIR}/src"
mkdir -p "${BUILD_DIR}/Resources"

# ── Step 1: prepare sources ───────────────────────────────────────────────────

if [[ $# -gt 0 ]]; then
    echo "[JQML v2] Preparing sources..."
    "${PYTHON}" "${PREPARE}" "$@"
fi

# ── Step 2: compile ───────────────────────────────────────────────────────────

echo "[JQML v2] Compiling QML → JavaScript..."
"${NODE}" "${COMPILER}" "${BUILD_DIR}/src"

echo "[JQML v2] Done: ${BUILD_DIR}/src/jqml.full.js"
