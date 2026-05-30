# JQML v3 – QML-to-Web Compiler

## Overview

The JQML v3 compiler is a Node.js-based transpiler that converts Qt QML source files into
browser-compatible JavaScript. It enables Qt developers to deploy QML applications to the web
without rewriting them in a different UI framework.

**Entry point:** `compiler.js`  
**Parser:** `parser.js`  
**Runtime bundle:** `../dist/main.js` (built from `../core/` via Webpack)

---

## Architecture

```
Tools/JQML/v3/
├── compiler/
│   ├── compiler.js      Main orchestrator (2,133 lines)
│   └── parser.js        QML/JS parser based on UglifyJS (1,935 lines)
├── core/                Runtime engine (JQApplication, JQContext, controllers)
├── dist/main.js         Webpack-bundled runtime
├── Qt/                  Qt global module (enums, utility functions)
├── QtBase/              Base class infrastructure
├── QtQml/               ~60 core types (Color, Font, Int, Anchors, …)
├── QtQuick/             ~50 UI components (Item, Rectangle, Text, ListView, …)
├── Qt5Compat/           Qt 5 compatibility layer
├── QtLocation/          Map types (Map, MapCircle, MapPolygon, …)
├── QtPositioning/       GeoCoordinate, positioning
├── QtWebSockets/        WebSocket types
├── webpack.config.js    Webpack config for runtime bundle
├── package.json
└── jqcompiler.bat       Windows entry script
```

---

## Compilation Pipeline

### Phase 1 – CLI Parsing and Configuration

The compiler is invoked from the command line (or from `jqcompiler.bat` on Windows):

```
node compiler/compiler.js
    -c <config.json>   path to the JSON config file (required)
    -o <dir>           output directory
    -e <file>          entry QML file
    -n <name>          application/bundle name
    -r <root>          URL root path (default: /)
    -m <mode>          output mode: "js" | "html"
    -i <file>          icon path (html mode only)
```

When `--mode html` is used, the compiler only writes a boilerplate HTML page and exits.
All further phases apply only to `--mode js`.

### Phase 2 – Config File Loading

A JSON config file declares:

| Field      | Description |
|------------|-------------|
| `dirs`     | Array of paths to `qmldir` module directories |
| `includes` | Array of additional config files to merge |
| `output`   | (legacy) output path |

Environment variables in the form `${VAR}` are substituted before parsing.
Nested config files referenced via `includes` are merged recursively so that
`dirs` entries are collected from all included configs.

Example (`imtcore.json`):
```json
{
    "dirs": [
        "${IMTCOREDIR}/Qml/imtcontrols",
        "${IMTCOREDIR}/Qml/imtgui"
    ],
    "includes": []
}
```

### Phase 3 – Module Discovery

Each directory listed in `dirs` must contain a `qmldir` file.  
Supported `qmldir` line formats:

```
module <ModuleName>
<ClassName> <version> <file.qml>
<ClassName> <version> <file.js>
singleton <ClassName> <version> <file.qml>
```

The compiler parses every referenced file into a `QmlFile` (for `.qml`) or `JSFile`
(for `.js`) object and registers it in the global `JQModules` registry.
When multiple versions of the same class exist, the highest version is used as the
default; specific versions remain accessible as `ClassName_v<version>`.

### Phase 4 – Single-File Parsing

All `.qml` files found in the directory of the entry file are parsed as *single files*
(i.e. local components of the application, not module components) and stored in
`SingleFiles`.

### Phase 5 – Code Generation

Each `QmlFile` calls `toCode()`, which produces a `SourceNode` tree (preserving
source location for source maps).  The generated code follows this pattern:

```javascript
class <ClassName> extends <ParentClass> {
    static cachedComponents = {}
    static meta = Object.assign({}, <ParentClass>.meta, { /* properties */ })

    static create(parent, properties = {}, context = {}, isRoot = true) {
        let __context = JQContext.create()
        let __el0 = super.create(parent, properties, context, false)
        __el0.__<ClassName>Context = __context

        // property assignments
        __el0.__proxy.width = 800
        __el0.__proxy.color = `blue`

        // child elements
        let __el1 = JQModules.QtQuick.Text.create(__el0, {})
        __el1.__proxy.text = `Hello`

        if (isRoot) {
            __el0.__updatePrimaryProperties()
            __el0.__updateProperties()
            __el0.__complete()
            __el0.__completeProperties()
        }
        return __el0
    }

    // methods …
    // signal connections …
}
```

Singleton classes are wrapped with an immediately-invoked class expression:

```javascript
JQModules.<Module>.<ClassName> = (class extends <ParentClass> {
    static singleton = true
    static create(…) { … }
}).create()
```

### Phase 6 – Dependency Ordering

Before emitting code, the compiler topologically sorts `compiledFiles` so that
every file is written after all files it depends on.  Singletons are always
emitted last.

### Phase 7 – Output

The final bundle is assembled as:

1. `dist/main.js` – the entire runtime (JQApplication, JQContext, all Qt module
   implementations)
2. Module namespace initialisers (`JQModules.<Name>={}`)
3. Compiled third-party module classes
4. Compiled single-file (application) classes
5. Compiled singletons
6. Bootstrap call:
   ```javascript
   window.addEventListener('load', () => {
       JQApplication.rootPath = '<root>'
       <EntryClass>.create(JQApplication.root)
   })
   ```

Two files are written to `<output>/<name>.js` and `<output>/<name>.js.map`.

---

## Key Classes

### `Instruction`

Represents a single QML element (one `{ … }` block).

| Member | Description |
|--------|-------------|
| `className` | QML type name (e.g. `Rectangle`) |
| `extends` | Resolved parent class path |
| `id` | Value of the `id:` property |
| `children` | Nested `Instruction` objects |
| `defineProperties` | `property <type> <name>` declarations |
| `defineSignals` | `signal <name>(…)` declarations |
| `defineMethods` | `function <name>(…)` definitions |
| `connectedSignals` | `on<Signal>: …` handlers |
| `assignProperties` | Property value assignments |

Key methods:

| Method | Description |
|--------|-------------|
| `qmlelem(meta)` | Adds a child element |
| `qmlpropdef(meta)` | Processes a property declaration |
| `qmlprop(meta)` | Processes a property assignment |
| `qmlsignaldef(meta)` | Processes a signal declaration |
| `qmlmethod(meta)` | Processes a method definition |
| `qmlaliasdef(meta)` | Processes an alias declaration |
| `resolve(name)` | Scope-aware identifier resolution |
| `prepare(tree)` | Transforms an AST subtree into JS source |
| `toCode()` | Generates the complete element code |
| `getMeta()` | Generates the `meta` object literal |

#### `resolve` – Identifier Resolution

Identifiers are resolved in the following priority order:

1. Locally declared properties, signals, or methods of the current element
2. Inherited members from the parent class (`getTypeInfo` + meta lookup)
3. Context variables (element `id` values in scope)
4. Singleton objects
5. Module types (`JQModules.<Module>.<Class>`)

#### `prepare` – AST-to-JS Transformation

`prepare` recursively walks the UglifyJS AST and emits JS source via `SourceNode`.
It handles:

- Literals: `num`, `string`, `regexp`
- Operators: `binary`, `unary-prefix`, `unary-postfix`, `assign`, `conditional`
- Control flow: `if`, `while`, `for`, `return`, `break`, `continue`, `throw`, `try`
- Expressions: `call`, `dot`, `sub`, `new`, `array`, `object`, `function`
- Declarations: `var`, `const`, `let`, `block`
- QML-specific: `qmlelem`, `qmlbinding` (reactive binding expressions)

### `QmlFile`

Wraps a single `.qml` source file.

| Member | Description |
|--------|-------------|
| `fileName` | Absolute path to the source file |
| `moduleName` | Module namespace (empty for single files) |
| `version` | Component version |
| `singleton` | `true` when `pragma Singleton` is declared |
| `instruction` | Root `Instruction` |
| `imports` | Parsed import statements |
| `dependencies` | Set of other `QmlFile` objects this file depends on |
| `context` | Map of `id` → `Instruction` for all elements in this file |

### `JSFile`

Wraps a `.js` file exported from a QML module.  
`toCode()` returns an IIFE that exposes the file's exported symbols:

```javascript
function() {
    /* file source */
    return { export1, export2 }
}()
```

---

## Parser (`parser.js`)

The parser is a modified version of the UglifyJS 2 parser, extended with QML-specific
grammar rules.  It produces an AST where QML constructs are represented as arrays whose
first element is a string tag:

| Tag | Meaning |
|-----|---------|
| `qmlelem` | `TypeName { … }` element |
| `qmlobj` | `TypeName.sub { … }` grouped sub-object |
| `qmlprop` | `name: value` property assignment |
| `qmlpropdef` | `property <type> name` declaration |
| `qmlsignaldef` | `signal name(…)` declaration |
| `qmlmethod` | `function name(…) { … }` method |
| `qmlaliasdef` | `property alias name: target` |
| `qmlenumdef` | `enum Name { … }` |
| `qmlimport` | `import Module version [as alias]` |
| `qmlpragma` | `pragma Directive` |

Every AST node carries an `info` object with `{ line, col, file }` for source-map
generation.

---

## Module System

### Built-in Modules

Loaded unconditionally at compiler startup:

| Module | Contents |
|--------|----------|
| `Qt` | Enums, `Qt.rgba()`, `Qt.lighter()`, `Qt.binding()`, etc. |
| `QtQml` | `Component`, `Connections`, `Binding`, `Timer`, property types |
| `QtQuick` | `Item`, `Rectangle`, `Text`, `Image`, `MouseArea`, animations, layouts |
| `Qt5Compat` | Compatibility shims for Qt 5 APIs |
| `QtWebSockets` | `WebSocket` |
| `QtPositioning` | `GeoCoordinate`, coordinate utilities |
| `QtLocation` | `Map`, `MapCircle`, `MapPolygon`, `MapPolyline` |

### Third-Party Modules

Declared in the `dirs` array of the config file.  Each directory contains a `qmldir`
file that lists the module name and its component files.  These are compiled into
`JQModules.<ModuleName>.<ClassName>` namespace paths.

---

## Runtime Engine (`core/`)

The runtime (bundled into `dist/main.js` by Webpack) provides:

| Module | Responsibility |
|--------|---------------|
| `JQApplication` | Global lifecycle, `root` DOM node, `rootPath` |
| `JQContext` | Execution scope, property binding, signal connections |
| `MouseController` | Mouse event dispatch |
| `KeyboardController` | Keyboard event dispatch |
| `ImageController` | Image loading and caching |
| `TextController` | Text measurement |
| `MemoryController` | Object lifecycle / garbage collection |

---

## Build Commands

```bash
# Install dependencies
npm install

# Build the runtime bundle (production)
npm run build-core

# Build the runtime bundle (development, with source maps)
npm run build-core-dev

# Run the compiler
npm run compile -- -c <config.json> -e <entry.qml> -o <outdir> -n <name> -m js
```

On Windows use the provided wrapper:
```bat
jqcompiler.bat -c <config.json> -e <entry.qml> -o <outdir> -n <name> -m js
```

---

## Integration with QMake (ImtCore)

The compiler is invoked automatically during the Qt build via
`Config/QMake/WebCompiler.pri`.  The `.pri` file uses `QMAKE_EXTRA_COMPILERS` to
create a custom build step that runs the compiler whenever QML sources change and
generates a `.qrc` file which is then compiled into the application binary as a Qt
resource.

---

## Dependencies

| Package | Version | Purpose |
|---------|---------|---------|
| `commander` | ^14.0.1 | CLI argument parsing |
| `source-map-generator` | ^0.8.0 | Source map generation |
| `uglify-js` | ^3.19.3 | JS AST (basis for the QML parser) |
| `ol` | ^7.5.2 | OpenLayers (used by QtLocation runtime) |
| `webpack-cli` | ^5.1.4 | Bundling the runtime (dev dependency) |
