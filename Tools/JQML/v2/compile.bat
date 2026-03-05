@echo off
REM SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
REM
REM JQML v2 — standalone compilation helper (Windows)
REM
REM Runs the full QML-to-JavaScript pipeline:
REM   1. preparesources.py    — copies QML directories into the build tree
REM   2. compiler\compiler.js — compiles the gathered sources to jqml.full.js
REM
REM Usage:
REM   compile.bat <build-dir> [<src1> <dst1> ...]
REM
REM Arguments:
REM   build-dir   Root of the web build directory.
REM               The JS bundle is written to <build-dir>\src\jqml.full.js.
REM   srcN dstN   Source / destination pairs forwarded to preparesources.py.
REM
REM Environment variables:
REM   NODE_EXE    Override Node.js executable
REM               (default: %IMTCOREDIR%\3rdParty\nodejs\node.exe, then node)
REM   PYTHONEXE   Override Python executable (default: python.exe)
REM   JQML_VERBOSE=1  Enable verbose output from the compiler
REM
REM Example:
REM   compile.bat C:\build\myapp\web ^
REM       C:\ImtCore\Qml\imtgui     C:\build\myapp\web\src\imtgui ^
REM       C:\ImtCore\Qml\imtcontrols C:\build\myapp\web\src\imtcontrols

setlocal enabledelayedexpansion

set "SCRIPT_DIR=%~dp0"
set "COMPILER=%SCRIPT_DIR%compiler\compiler.js"
set "PREPARE=%SCRIPT_DIR%preparesources.py"

REM ── Resolve executables ───────────────────────────────────────────────────────

if not defined NODE_EXE (
    if defined IMTCOREDIR (
        set "NODE_EXE=%IMTCOREDIR%\3rdParty\nodejs\node.exe"
    )
    if not exist "%NODE_EXE%" set "NODE_EXE=node"
)

if not defined PYTHONEXE set "PYTHONEXE=python.exe"

REM ── Validate arguments ────────────────────────────────────────────────────────

if "%~1"=="" (
    echo Usage: %~nx0 ^<build-dir^> [^<src1^> ^<dst1^> ...] >&2
    exit /b 2
)

set "BUILD_DIR=%~1"
shift

REM ── Prepare output directories ────────────────────────────────────────────────

if not exist "%BUILD_DIR%\src"       mkdir "%BUILD_DIR%\src"
if not exist "%BUILD_DIR%\Resources" mkdir "%BUILD_DIR%\Resources"

REM ── Step 1: prepare sources ───────────────────────────────────────────────────

if not "%~1"=="" (
    echo [JQML v2] Preparing sources...
    "%PYTHONEXE%" "%PREPARE%" %*
)

REM ── Step 2: compile ───────────────────────────────────────────────────────────

echo [JQML v2] Compiling QML -^> JavaScript...
"%NODE_EXE%" "%COMPILER%" "%BUILD_DIR%\src"

echo [JQML v2] Done: %BUILD_DIR%\src\jqml.full.js
endlocal
