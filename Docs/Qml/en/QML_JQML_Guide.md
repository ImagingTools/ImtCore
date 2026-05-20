# JQML Guide — QML for the Web

JQML is ImtCore's QML-to-JavaScript compiler toolchain. It enables you to run the same QML source code both as a native Qt desktop application and as a web application in the browser.

---

## Overview

```
┌──────────────────────┐
│    QML Source Code    │   ← Write once
│  (imtcontrols, etc.) │
└──────┬───────────────┘
       │
       ├──→ Qt Desktop Build (native)
       │
       └──→ JQML Compiler ──→ JavaScript ──→ Browser
```

### JQML Versions

| Version | Directory | Status | Description |
|---------|-----------|--------|-------------|
| **v1** | `Tools/JQML/v1/` | Legacy | PureQML — first generation |
| **v2** | `Tools/JQML/v2/` | Active | JQML2 — with 3D support, 130+ components |
| **v3** | `Tools/JQML/v3/` | Active | JQ — new architecture, custom parser, Source Maps |

---

## JQML v2 — JQML2

### Architecture

```
Tools/JQML/v2/
├── compiler/           ← QML-to-JS Compiler
├── components/         ← 130+ Component Implementations (JS)
│   ├── Item.js         ← Base Item
│   ├── Rectangle.js    ← Rectangle
│   ├── Text.js         ← Text
│   ├── ListView.js     ← List View
│   ├── View3D.js       ← 3D Scene (Three.js)
│   ├── TreeItemModel.js ← Tree Model
│   └── list.js         ← Component Registry
├── core/               ← Runtime Engine
├── utils/              ← Utility Functions
├── dist/               ← Compiled Bundle
├── package.json
└── webpack.config.js
```

### Compilation

```bash
cd Tools/JQML/v2

# Install dependencies
npm install

# Build runtime bundle (production)
npm run build-core

# Build runtime bundle (development, with Source Maps)
npm run build-core-dev

# Compile QML project
node compiler/compiler.js \
    -c config.json \
    -e MainApp.qml \
    -o output/ \
    -n myapp \
    -m js
```

### Component Mapping (v2)

JQML v2 implements over 130 components as JavaScript classes:

| Category | Components |
|----------|------------|
| **Basic** | Item, Rectangle, Text, TextInput, TextEdit, Image, MouseArea, FocusScope |
| **Layout** | Column, Row, Flow, SplitView, Repeater |
| **Lists** | ListView, GridView, TableView, TreeView |
| **Models** | ListModel, ListElement, TableModel, TreeItemModel, GqlModel |
| **Animation** | Animation, NumberAnimation, PropertyAnimation, PauseAnimation |
| **Dialogs** | Popup, Drawer, Menu, MenuItem, MenuBar, FileDialog, FolderDialog |
| **3D** | View3D, Node3D, Model, PerspectiveCamera, DirectionalLight, PrincipledMaterial |
| **Input** | DragHandler, WheelHandler, Shortcut |
| **Network** | WebSocket, WebView |
| **Maps** | Map, MapCircle, MapPolygon, MapPolyline, MapQuickItem |
| **Utility** | Timer, Settings, Component, Connections, Binding, Loader |

### 3D Support

JQML v2 includes 3D visualization via Three.js (MIT license):

```qml
import imt3dgui 1.0

View3D {
    width: 800
    height: 600

    PerspectiveCamera {
        position: Qt.vector3d(0, 200, 300)
    }

    DirectionalLight {
        eulerRotation: Qt.vector3d(-30, -30, 0)
    }

    Model {
        source: "#Cube"
        materials: PrincipledMaterial {
            baseColor: "steelblue"
        }
    }
}
```

**Mapping Qt Quick 3D ↔ Three.js:**

| QML Type | Three.js Type |
|----------|---------------|
| `View3D` | `WebGLRenderer` + `Scene` |
| `Node3D` | `Object3D` / `Group` |
| `Model` | `Mesh` |
| `PerspectiveCamera` | `PerspectiveCamera` |
| `DirectionalLight` | `DirectionalLight` |
| `PrincipledMaterial` | `MeshStandardMaterial` |

Mesh primitives: `"#Cube"`, `"#Sphere"`, `"#Cylinder"`, `"#Cone"`, `"#Plane"`, `"#Rectangle"`

glTF files (`.glb`/`.gltf`) are loaded asynchronously via `GLTFLoader`.

➡️ Details: [Tools/JQML/v2/README-3D.md](../../Tools/JQML/v2/README-3D.md)

### Adding a New Component to JQML v2

1. Create a JavaScript file in `Tools/JQML/v2/components/`
2. Register it in `components/list.js`
3. Done — no compiler code needed

```javascript
// components/MyComponent.js
class MyComponent extends Item {
    // Implementation
}
```

### TreeItemModel API Compatibility

When accessing TreeItemModel methods, both uppercase and lowercase must be supported:

| Qt/C++ (Uppercase) | JQML (Lowercase) |
|--------------------|-------------------|
| `GetTreeItemModel` | `getTreeItemModel` |
| `GetItemsCount` | `getItemsCount` |
| `GetData` | `getData` |

---

## JQML v3 — JQ

### Architecture

```
Tools/JQML/v3/
├── compiler/
│   ├── compiler.js     ← Main Compiler (2,133 lines)
│   └── parser.js       ← QML Parser based on UglifyJS (1,935 lines)
├── core/               ← Runtime Engine
│   ├── JQApplication   ← Global Lifecycle
│   ├── JQContext        ← Execution Scope, Bindings
│   ├── MouseController  ← Mouse Event Dispatch
│   ├── KeyboardController ← Keyboard Event Dispatch
│   ├── ImageController  ← Image Loading and Caching
│   ├── TextController   ← Text Measurement
│   └── MemoryController ← Object Lifecycle
├── dist/main.js        ← Webpack Bundle (Runtime)
├── Qt/                 ← Qt Global Module (Enums, Utility Functions)
├── QtBase/             ← Base Class Infrastructure
├── QtQml/              ← ~60 Core Types
├── QtQuick/            ← ~50 UI Components
├── Qt5Compat/          ← Qt 5 Compatibility Layer
├── QtLocation/         ← Map Types
├── QtPositioning/      ← Geo Coordinates
├── QtWebSockets/       ← WebSocket
└── package.json
```

### Compilation Pipeline

The v3 compiler goes through 7 phases:

```
Phase 1: CLI Parsing & Configuration
    ↓
Phase 2: Load Config File (JSON with dirs, includes)
    ↓
Phase 3: Module Discovery (parse qmldir files)
    ↓
Phase 4: Single-File Parsing (App Components)
    ↓
Phase 5: Code Generation (QML → JavaScript Classes)
    ↓
Phase 6: Dependency Sorting (topological)
    ↓
Phase 7: Output (Bundle + Source Map)
```

### Configuration

Config file (JSON):

```json
{
    "dirs": [
        "${IMTCOREDIR}/Qml/imtcontrols",
        "${IMTCOREDIR}/Qml/imtgui",
        "${IMTCOREDIR}/Qml/imtguigql"
    ],
    "includes": [
        "other-config.json"
    ]
}
```

Environment variables (`${VAR}`) are automatically substituted.

### Build Commands

```bash
cd Tools/JQML/v3

# Install dependencies
npm install

# Build runtime bundle (production)
npm run build-core

# Build runtime bundle (development)
npm run build-core-dev

# Compile QML project
npm run compile -- \
    -c config.json \
    -e MainApp.qml \
    -o output/ \
    -n myapp \
    -m js
```

Windows:
```bat
jqcompiler.bat -c config.json -e MainApp.qml -o output -n myapp -m js
```

### HTML Mode

The compiler can also generate a boilerplate HTML page:

```bash
npm run compile -- -c config.json -e MainApp.qml -o output -n myapp -m html -i icon.png
```

### Built-in Modules

| Module | Contents |
|--------|----------|
| `Qt` | Enums, `Qt.rgba()`, `Qt.lighter()`, `Qt.binding()` |
| `QtQml` | Component, Connections, Binding, Timer, Property Types |
| `QtQuick` | Item, Rectangle, Text, Image, MouseArea, Animations, Layouts |
| `Qt5Compat` | Qt 5 Compatibility Shims |
| `QtWebSockets` | WebSocket |
| `QtPositioning` | GeoCoordinate |
| `QtLocation` | Map, MapCircle, MapPolygon, MapPolyline |

### QMake Integration

The JQML compiler is automatically executed during the Qt build via `Config/QMake/WebCompiler.pri`:

- Uses `QMAKE_EXTRA_COMPILERS` for a custom build step
- Compiles on changes to QML files
- Generates a `.qrc` file that is embedded into the application

---

## Cross-Platform Development

### Principles

1. **One QML source code** for desktop and web
2. **Modules via qmldir** — the compiler finds components automatically
3. **No platform-specific imports** in business logic code

### Platform-Specific Code

For cases where desktop and web require different behavior:

```
Qml/imtcontrols/Base/         ← Default (Desktop)
Qml/imtcontrols/Base/web/     ← Web Overrides
```

Web-specific files in `web/` subdirectories override their desktop counterparts:
- `ScrollBar.qml` → Web-compatible scroll bar
- `ToolTip.qml` → Web-compatible tooltip
- `Switch.qml` → Web-compatible switch
- `Window.qml` → Web-compatible window wrapper

### QML Rules for JQML Compatibility

#### ✅ Do

- Use standard QML properties and declarative bindings
- `ListModel` for static data
- Data provider pattern for dynamic data
- `import imtcontrols 1.0` instead of direct Qt imports where possible

#### ❌ Avoid

- **`prop: { ... JS block ... }`** for model bindings
  ```qml
  // ❌ WRONG
  // model: { return computeModel() }

  // ✅ CORRECT
  model: myDataProvider.items
  ```

- Desktop-specific Qt types without a JQML equivalent
- Direct C++ calls without a QML wrapper layer
- `Qt.createComponent()` in performance-critical code

#### TreeItemModel Access

Check for both uppercase and lowercase:

```qml
// In TreeModelBuilder
function childModel(item) {
    if (typeof item.GetTreeItemModel === "function")
        return item.GetTreeItemModel()
    if (typeof item.getTreeItemModel === "function")
        return item.getTreeItemModel()
}
```

### Dependencies (v3)

| Package | Version | Purpose |
|---------|---------|---------|
| `commander` | ^14.0.1 | CLI argument parsing |
| `source-map-generator` | ^0.8.0 | Source map generation |
| `uglify-js` | ^3.19.3 | JS AST (basis for QML parser) |
| `ol` | ^7.5.2 | OpenLayers (QtLocation runtime) |
| `webpack-cli` | ^5.1.4 | Runtime bundling (dev dependency) |

---

## Debugging

### Source Maps

JQML v3 generates source maps (`.js.map`) that allow browser DevTools to debug directly in the QML source code.

### Render Strategy

`View3D` (v2 3D) uses **on-demand rendering**: Only property changes trigger a `requestAnimationFrame`. Idle scenes consume no CPU/GPU.

### Picking

`View3D` provides:
- `pick(x, y)` — nearest `Node3D` under 2D coordinate
- `mapFrom3DScene({x, y, z})` — world → screen
- `mapTo3DScene({x, y, z})` — screen → world

---

## Further Documentation

- **[Tools/JQML/v2/README-3D.md](../../Tools/JQML/v2/README-3D.md)** — JQML2 3D Documentation
- **[Tools/JQML/v3/compiler/README.md](../../Tools/JQML/v3/compiler/README.md)** — JQML v3 Compiler Architecture
- **[QML Documentation Index](QML_Documentation_Index.md)** — Documentation Overview
- **[QML Module Reference](QML_Module_Reference.md)** — All QML Components
- **[QML Getting Started](QML_Getting_Started.md)** — Getting Started Tutorial
- **[QML Component Tutorials](QML_Component_Tutorials.md)** — Practical Tutorials
