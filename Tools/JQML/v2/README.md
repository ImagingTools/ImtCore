# JQML v2 — QML-to-JavaScript Compiler Tools

This directory contains the tools for the **JQML v2** QML-to-JavaScript
compiler used to build web deployments of ImtCore Qt/QML applications.

## Contents

```
v2/
├── compiler/            Node.js compiler (see compiler/README.md)
│   ├── compiler.js      Entry point: node compiler.js <src-dir>
│   ├── package.json
│   ├── src/
│   │   ├── lexer.js     QML tokenizer
│   │   ├── parser.js    Recursive-descent QML parser
│   │   ├── codegen.js   JavaScript code generator
│   │   └── bundler.js   File-system walker + JQML runtime header
│   └── test/
│       └── run.js       Self-contained test suite
├── preparesources.py    Python script: copies QML dirs into the build tree
├── compile.sh           Convenience wrapper (Linux / macOS)
└── compile.bat          Convenience wrapper (Windows)
```

## Quick start

### Prerequisites

| Tool    | Purpose                        | Min. version |
|---------|--------------------------------|--------------|
| Node.js | Runs the compiler              | 14.0.0       |
| Python  | Runs `preparesources.py`       | 3.7          |

### Run the tests

```bash
node compiler/test/run.js
```

### Compile a directory of QML files

```bash
# Linux / macOS
./compile.sh /path/to/build/web \
    /path/to/Qml/imtgui      /path/to/build/web/src/imtgui \
    /path/to/Qml/imtcontrols /path/to/build/web/src/imtcontrols

# Windows
compile.bat C:\build\web ^
    C:\ImtCore\Qml\imtgui      C:\build\web\src\imtgui ^
    C:\ImtCore\Qml\imtcontrols C:\build\web\src\imtcontrols
```

The output bundle is written to `<build-dir>/src/jqml.full.js`.

## Pipeline

```
Source QML files (multiple directories)
        │
        ▼
preparesources.py   ← copies everything into <build-dir>/src/
        │
        ▼
compiler/compiler.js
        │
        ├─ lexer.js    tokenise each .qml file
        ├─ parser.js   build an AST
        ├─ codegen.js  emit JQML.define(...) calls
        └─ bundler.js  concatenate runtime + JS helpers + components
        │
        ▼
<build-dir>/src/jqml.full.js   (self-contained, loads in any browser)
        │
        ▼
rcc -name <TargetName>Web <TargetName>JsWeb.qrc -o qrc_<TargetName>Web.cpp
        │
        ▼
Linked into the Qt application as a Qt resource
```

## Build system integration

For full integration instructions see
[`Docs/WebCompiler_Guide.md`](../../Docs/WebCompiler_Guide.md).

### CMake (quick reference)

```cmake
include(${IMTCOREDIR}/Config/CMake/WebCompiler.cmake)

set(buildwebdir ${CMAKE_CURRENT_BINARY_DIR}/web)
set(resname     myapp)

# Collect source dirs (pairs: source → destination)
getImtCoreQmlWebDirs(webdirs ${buildwebdir})
list(APPEND webdirs ${CMAKE_CURRENT_SOURCE_DIR}/Qml)
list(APPEND webdirs ${buildwebdir}/src/myapp)

jqml_compile_web2()
```

### QMake (quick reference)

```qmake
include($(IMTCOREDIR)/Config/QMake/WebCompiler.pri)
include($(IMTCOREDIR)/Config/QMake/QmlControls.pri)

BUILDWEBDIR = $$OUT_PWD/web

prepareWebQml($$BUILDWEBDIR)
copyToWebDir($$PWD/Qml, $$BUILDWEBDIR/src/myapp)
compyleWeb($$BUILDWEBDIR, myapp)

include($(IMTCOREDIR)/Config/QMake/WebQrc.pri)
GENERATED_RESOURCES += dummy
```

## Compiler API (runtime)

After loading `jqml.full.js` in a browser, the global `JQML` object is
available:

```js
// Instantiate a registered component
const btn = JQML.create("MyButton", { label: "Save" });

// Read a property (resolves bindings)
console.log(btn.get("label")); // "Save"

// Set a property (fires onLabelChanged if defined)
btn.set("label", "Cancel");

// Emit a signal (invokes matching onXxx handler)
btn.emit("clicked");

// Call a QML function
btn.call("reset");

// Fire Component.onCompleted
btn.complete();
```
