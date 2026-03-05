# QML-to-JS Web Compiler Guide

Complete guide to building web deployments of ImtCore Qt/QML applications
using the JQML v2 compiler.

---

## Table of contents

1. [Overview](#1-overview)
2. [Architecture](#2-architecture)
3. [Prerequisites](#3-prerequisites)
4. [Compilation pipeline](#4-compilation-pipeline)
5. [Compiler variants](#5-compiler-variants)
6. [CMake integration](#6-cmake-integration)
7. [QMake integration](#7-qmake-integration)
8. [Adding QML source directories](#8-adding-qml-source-directories)
9. [Output structure](#9-output-structure)
10. [Runtime API](#10-runtime-api)
11. [Configuration reference](#11-configuration-reference)
12. [Troubleshooting](#12-troubleshooting)

---

## 1. Overview

ImtCore applications are written in C++ and QML. The web compiler pipeline
converts the QML UI layer into a self-contained JavaScript bundle
(`jqml.full.js`) that runs in any modern browser. The bundle is compiled into
the Qt application as a Qt resource (via `rcc`) so the web frontend is served
directly from the binary without a separate deployment step.

```
QML source files
      │
      ▼
preparesources.py ──► build/src/  (copies QML trees)
      │
      ▼
compiler.js  ──────────────────────► build/src/jqml.full.js
      │
      ▼
rcc  ──────────────────────────────► qrc_<Target>Web.cpp
      │
      ▼
Linked into Qt application
```

---

## 2. Architecture

### Components

| Component | Location | Purpose |
|-----------|----------|---------|
| `preparesources.py` | `Tools/JQML/v2/` | Copies QML source trees into the build directory |
| `compiler.js` | `Tools/JQML/v2/compiler/` | Node.js QML→JS compiler (entry point) |
| `lexer.js` | `Tools/JQML/v2/compiler/src/` | QML tokenizer |
| `parser.js` | `Tools/JQML/v2/compiler/src/` | Recursive-descent QML parser |
| `codegen.js` | `Tools/JQML/v2/compiler/src/` | JavaScript code generator |
| `bundler.js` | `Tools/JQML/v2/compiler/src/` | File walker + output writer |
| `WebCompiler.cmake` | `Config/CMake/` | CMake build integration functions |
| `WebCompiler.pri` | `Config/QMake/` | QMake build integration functions |
| `WebQrc.pri` | `Config/QMake/` | QMake QRC compilation step |
| `QmlControls.pri` | `Config/QMake/` | Standard ImtCore source directory list |

### Core QML source directories

| Directory | Module | Description |
|-----------|--------|-------------|
| `Qml/imtgui` | `imtgui` | Base UI elements, views, panels |
| `Qml/imtguigql` | `imtguigql` | GraphQL-aware UI components |
| `Qml/imtcontrols` | `imtcontrols` | Input controls, buttons, lists |
| `Qml/imtcolgui` | `imtcolgui` | Collection browsing components |
| `Qml/imtdocgui` | `imtdocgui` | Document management UI |
| `Qml/imtlicgui` | `imtlicgui` | Licensing UI |
| `Qml/imtauthgui` | `imtauthgui` | Authentication UI |
| `Qml/web` | — | Web-specific JS helpers (`GraphQLRequest.js`, etc.) |

---

## 3. Prerequisites

| Tool | Minimum | Purpose |
|------|---------|---------|
| Node.js | 14.0.0 | Runs `compiler.js` |
| Python | 3.7 | Runs `preparesources.py` |
| Qt `rcc` | Qt 5.12 / Qt 6 | Compiles the `.qrc` file to C++ |

### Windows

On Windows, Node.js is expected at
`%IMTCOREDIR%\3rdParty\nodejs\node.exe` (bundled with ImtCore).  
Python is looked up via `%PYTHONEXE%` or defaults to `python.exe` on `PATH`.

### Linux / macOS

`node` and `python3` must be on `PATH`, or override with environment
variables `NODE_EXE` and `PYTHONEXE`.

---

## 4. Compilation pipeline

The full pipeline runs automatically as part of the build when you include the
relevant CMake or QMake integration files. The steps are:

### Step 1 — Prepare sources

`preparesources.py` is called with a flat list of *(source, destination)*
directory pairs. It copies each source tree into the corresponding destination
inside the build directory.

```
Qml/imtgui  ──► build/web/src/imtgui
Qml/imtcontrols ──► build/web/src/imtcontrols
...
SDL/generated/*.qml ──► build/web/src/<module>
```

### Step 2 — Compile QML → JavaScript

`compiler.js` walks `build/web/src/` recursively:

- **`.qml` files** — parsed through the lexer → parser → code-generator
  pipeline. Each component becomes a `JQML.define()` call.
- **`.js` files** — included verbatim (they contain web-specific helpers like
  `GraphQLRequest.js`).

All output is concatenated (with the JQML runtime prepended) into
`build/web/src/jqml.full.js`.

### Step 3 — Copy to Resources

`jqml.full.js` is copied to `build/web/Resources/jqml.<resname>.js`.

### Step 4 — Generate QRC

`rcc` compiles the `.qrc` resource manifest into a C++ source file
`qrc_<Target>Web.cpp`.

### Step 5 — Link

The generated C++ file is added to `target_sources()` / `SOURCES` and linked
into the Qt application.

---

## 5. Compiler variants

ImtCore provides several compiler functions for different use cases.

### `jqml_compile_web2` (CMake) / `compyleWeb` (QMake) — **recommended**

JQML v2 compiler.  Uses `Tools/JQML/v2/preparesources.py` and
`Tools/JQML/v2/compiler/compiler.js`.  Generates `jqml.full.js`.

### `jqml_compile_web` (CMake)

JQML v1 compiler.  Uses `3rdParty/JQML/preparesources.py` and
`3rdParty/JQML/Compiler/index.js`.  Legacy — prefer v2 for new projects.

### `imtcore_compile_web` (CMake)

Python-based web compiler (`3rdParty/WebCompiler/webcompiler.py`).  Used for
projects that do not require the full JQML component model.

### `jq_compile_web` (CMake)

JQ compiler (`3rdParty/JQ`).  Supports both HTML and JS output modes and is
used for standalone web application targets.

---

## 6. CMake integration

### Include the module

```cmake
include(${IMTCOREDIR}/Config/CMake/WebCompiler.cmake)
```

### Minimal example

```cmake
set(buildwebdir ${CMAKE_CURRENT_BINARY_DIR}/web)
set(resname     myapp)

# Build the list of (source, destination) pairs
getImtCoreQmlWebDirs(webdirs ${buildwebdir})

# Add your own QML directory
list(APPEND webdirs ${CMAKE_CURRENT_SOURCE_DIR}/Qml)
list(APPEND webdirs ${buildwebdir}/src/myapp)

# Run the JQML v2 pipeline
jqml_compile_web2()
```

This call:

1. Invokes `preparesources.py` with all entries in `webdirs`.
2. Runs `compiler.js` on `${buildwebdir}/src`.
3. Copies `jqml.full.js` to `${buildwebdir}/Resources/jqml.${resname}.js`.
4. Calls `rcc` to compile `${buildwebdir}/Resources/${PROJECT_NAME}JsWeb.qrc`
   into `${buildwebdir}/Resources/qrc_${PROJECT_NAME}Web.cpp`.
5. Adds the generated `.cpp` to the target's sources via `target_sources()`.

### Helper macros

| Macro | Description |
|-------|-------------|
| `getImtBaseQmlWebDirs(webdirs buildwebdir [useImtControlsStyle])` | Appends all core ImtBase + style QML directories |
| `getImtLicQmlWebDirs(webdirs buildwebdir)` | Appends licensing QML directories |
| `getImtAuthQmlWebDirs(webdirs buildwebdir)` | Appends authentication QML directories |
| `getImtCoreQmlWebDirs(webdirs buildwebdir [useImtControlsStyle])` | Calls all three above |

### SDL-generated QML

SDL-generated QML modules live under
`${IMTCOREDIR_BUILD}/AuxInclude/${TARGETNAME}/GeneratedFiles/`. They are
automatically included by the `getImtCore*QmlWebDirs` macros.  For custom SDL
modules, append the generated QML directory manually:

```cmake
list(APPEND webdirs
    ${IMTCOREDIR_BUILD}/AuxInclude/${TARGETNAME}/GeneratedFiles/mymodulesdl/SDL/1.0/QML/mymoduleDataSdl
    ${buildwebdir}/src/mymoduleDataSdl
)
```

### Dependency tracking

`jqml_compile_web2` uses `DEPENDS ${DEPEND_LIST}` which is built from
`file(GLOB_RECURSE ... "*.qml")` over all source directories.  Any change to a
`.qml` file triggers a rebuild.

---

## 7. QMake integration

### Include the modules

```qmake
include($(IMTCOREDIR)/Config/QMake/WebCompiler.pri)
include($(IMTCOREDIR)/Config/QMake/QmlControls.pri)
```

### Minimal example

```qmake
BUILDWEBDIR = $$OUT_PWD/web

# Copy standard ImtCore QML modules
prepareWebQml($$BUILDWEBDIR)

# Copy your own QML directory
copyToWebDir($$PWD/Qml, $$BUILDWEBDIR/src/myapp)

# Run the compiler and rcc
compyleWeb($$BUILDWEBDIR, myapp)

# Register the generated .cpp with QMake
include($(IMTCOREDIR)/Config/QMake/WebQrc.pri)
GENERATED_RESOURCES += dummy
```

### QMake functions

#### `copyToWebDir(src, dst)`

Appends a copy command to `WEB_COMMAND`.  The source directory is copied
recursively to the destination.

```qmake
copyToWebDir($$PWD/MyQml, $$BUILDWEBDIR/src/myqml)
```

#### `copyFile(src, dst)`

Appends a single-file copy to `WEB_COMMAND`.

```qmake
copyFile($$PWD/extra.js, $$BUILDWEBDIR/src/extra.js)
```

#### `compyleWeb(buildwebdir, resname)`

Appends the Node.js compilation and `rcc` commands to `WEB_COMMAND`.  Also
sets and exports `QRC_WEB_FILE` and `QRC_CPP_WEB_FILE` for use by
`WebQrc.pri`.

```qmake
compyleWeb($$BUILDWEBDIR, myapp)
# Sets QRC_WEB_FILE    = <buildwebdir>/Resources/<TARGET>JsWeb.qrc
# Sets QRC_CPP_WEB_FILE = <buildwebdir>/Resources/qrc_<TARGET>Web.cpp
```

### Windows note

On Windows, `compyleWeb` uses
`$(IMTCOREDIR)/3rdParty/JQML2/jqml2compiler.bat` instead of `npm` and adds
the bundled Node.js directory to `PATH`.

---

## 8. Adding QML source directories

### Adding a new application QML module

1. Create your QML files in e.g. `Qml/mymodule/`.

2. **CMake** — append a pair to `webdirs` before calling `jqml_compile_web2`:

   ```cmake
   list(APPEND webdirs
       ${CMAKE_CURRENT_SOURCE_DIR}/Qml/mymodule
       ${buildwebdir}/src/mymodule
   )
   jqml_compile_web2()
   ```

3. **QMake** — call `copyToWebDir` before `compyleWeb`:

   ```qmake
   copyToWebDir($$PWD/Qml/mymodule, $$BUILDWEBDIR/src/mymodule)
   compyleWeb($$BUILDWEBDIR, myapp)
   ```

### Adding an SDL-generated QML module

SDL code generation produces QML files under
`AuxInclude/<target>/GeneratedFiles/<module>/SDL/1.0/QML/<ModuleName>/`.

**CMake:**

```cmake
list(APPEND webdirs
    ${IMTCOREDIR_BUILD}/AuxInclude/${TARGETNAME}/GeneratedFiles/mymodulesdl/SDL/1.0/QML/mymoduleDataSdl
    ${buildwebdir}/src/mymoduleDataSdl
)
```

**QMake:**

```qmake
copyToWebDir($$imtcoredir/$$AUXINCLUDEDIR/GeneratedFiles/mymodulesdl/SDL/1.0/QML/mymoduleDataSdl,
             $$BUILDWEBDIR/src/mymoduleDataSdl)
```

---

## 9. Output structure

After a successful build the web directory contains:

```
<buildwebdir>/
├── src/
│   ├── imtgui/               Copied QML sources
│   ├── imtcontrols/
│   ├── myapp/
│   ├── ...
│   └── jqml.full.js          ← compiled JavaScript bundle
└── Resources/
    ├── jqml.myapp.js         Copy of jqml.full.js (named after resname)
    ├── <Target>JsWeb.qrc     Qt resource manifest
    └── qrc_<Target>Web.cpp   C++ resource file linked into the binary
```

The `.qrc` manifest references `jqml.<resname>.js` under the Qt resource path
`/<Target>Web/`.

---

## 10. Runtime API

Load `jqml.full.js` in a browser (or any JS environment) to gain access to the
`JQML` global:

```js
// List all compiled components
console.log(JQML.list());

// Instantiate a component
const editor = JQML.create("FeatureEditor", {
    featureId: "feature-42"
});

// Fire Component.onCompleted (initialises the component)
editor.complete();

// Read a property
const fid = editor.get("featureId");

// Set a property (triggers onFeatureIdChanged if defined)
editor.set("featureId", "feature-99");

// Call a QML function
const parents = editor.call("getAllParents", someIndex);

// Emit a signal
editor.emit("clicked");
```

---

## 11. Configuration reference

### CMake variables

| Variable | Default | Description |
|----------|---------|-------------|
| `buildwebdir` | (required) | Root build directory for web output |
| `resname` | (required) | Resource name used in the output filename |
| `webdirs` | (required) | List of (source, destination) pairs |
| `sdldependency` | `""` | Additional CMake target to depend on |
| `PYTHONEXE` | `python3` / `python.exe` | Python executable |
| `NODE_EXE` | `node` / bundled exe | Node.js executable |
| `IMTCOREDIR` | (required) | Root of the ImtCore checkout |
| `IMTCOREDIR_BUILD` | (required) | Root of the ImtCore build directory |
| `TARGETNAME` | (required) | Platform/toolchain identifier (e.g. `VC17_64`) |

### QMake variables

| Variable | Description |
|----------|-------------|
| `WEB_COMMAND` | Accumulated shell command built by `copyToWebDir`, `copyFile`, `compyleWeb` |
| `QRC_WEB_FILE` | Path of the generated `.qrc` file (set by `compyleWeb`, exported) |
| `QRC_CPP_WEB_FILE` | Path of the generated `.cpp` file (set by `compyleWeb`, exported) |
| `BUILDWEBDIR` | Conventional name for the build web root in your `.pro` file |

---

## 12. Troubleshooting

### `jqml.full.js` not generated

- Confirm `node` / `node.exe` is on `PATH` (or `NODE_EXE` is set correctly).
- Check that `preparesources.py` succeeded — if source directories are
  missing, `build/web/src/` will be empty and the compiler will produce an
  empty bundle.
- On Windows, verify `%IMTCOREDIR%\3rdParty\nodejs\node.exe` exists.

### `qrc_<Target>Web.cpp` not generated (QMake)

- The QRC file is generated at **qmake time** via a `system()` call.
  If `WEB_COMMAND` is empty when `compyleWeb` runs, the file will not be
  created.
- Make sure all `copyToWebDir` / `copyFile` calls happen **before**
  `compyleWeb`.
- On Windows, avoid using `&&` in `system()` calls — use separate
  `system()` statements (see stored conventions).

### Compilation errors in `.qml` files

Run the compiler directly to see per-file error messages:

```bash
JQML_VERBOSE=1 node Tools/JQML/v2/compiler/compiler.js /path/to/build/web/src
```

The compiler writes partial output even when errors occur, so you can inspect
`jqml.full.js` to identify which component failed.

Common issues:

| Symptom | Likely cause |
|---------|--------------|
| `Expected token RBRACE` | Unbalanced `{}` in a JS function body |
| Component not found at runtime | `.qml` filename does not match `JQML.define` name |
| Property binding not evaluated | Multi-line expression; wrap in `{ return ...; }` |
| Blank output for a component | Root element is not an `Identifier {` form (e.g. `pragma Singleton`) |

### Testing the compiler in isolation

```bash
cd Tools/JQML/v2
node compiler/test/run.js
```

All tests should report `passed`.

### Verifying the QRC manifest

```bash
cat build/web/Resources/<Target>JsWeb.qrc
```

It should reference `jqml.<resname>.js` with a valid `alias`.  If the file
is missing or empty, re-run qmake / CMake configure with `JQML_VERBOSE=1`.
