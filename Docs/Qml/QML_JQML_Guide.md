# JQML Guide — QML für das Web

JQML ist die QML-zu-JavaScript-Compiler-Toolchain von ImtCore. Sie ermöglicht es, denselben QML-Quellcode sowohl als native Qt-Desktop-Anwendung als auch als Web-Applikation im Browser auszuführen.

---

## Übersicht

```
┌──────────────────────┐
│    QML-Quellcode     │   ← Einmal schreiben
│  (imtcontrols, etc.) │
└──────┬───────────────┘
       │
       ├──→ Qt Desktop Build (native)
       │
       └──→ JQML Compiler ──→ JavaScript ──→ Browser
```

### JQML-Versionen

| Version | Verzeichnis | Status | Beschreibung |
|---------|------------|--------|-------------|
| **v1** | `Tools/JQML/v1/` | Legacy | PureQML — erste Generation |
| **v2** | `Tools/JQML/v2/` | Aktiv | JQML2 — mit 3D-Unterstützung, 130+ Komponenten |
| **v3** | `Tools/JQML/v3/` | Aktiv | JQ — neue Architektur, eigener Parser, Source Maps |

---

## JQML v2 — JQML2

### Architektur

```
Tools/JQML/v2/
├── compiler/           ← QML-zu-JS Compiler
├── components/         ← 130+ Komponent-Implementierungen (JS)
│   ├── Item.js         ← Basis-Item
│   ├── Rectangle.js    ← Rechteck
│   ├── Text.js         ← Text
│   ├── ListView.js     ← Listen-Ansicht
│   ├── View3D.js       ← 3D-Szene (Three.js)
│   ├── TreeItemModel.js ← Baum-Modell
│   └── list.js         ← Komponenten-Registry
├── core/               ← Runtime-Engine
├── utils/              ← Hilfsfunktionen
├── dist/               ← Kompiliertes Bundle
├── package.json
└── webpack.config.js
```

### Kompilierung

```bash
cd Tools/JQML/v2

# Abhängigkeiten installieren
npm install

# Runtime-Bundle bauen (Produktion)
npm run build-core

# Runtime-Bundle bauen (Entwicklung, mit Source Maps)
npm run build-core-dev

# QML-Projekt kompilieren
node compiler/compiler.js \
    -c config.json \
    -e MainApp.qml \
    -o output/ \
    -n myapp \
    -m js
```

### Komponenten-Mapping (v2)

JQML v2 implementiert über 130 Komponenten als JavaScript-Klassen:

| Kategorie | Komponenten |
|-----------|------------|
| **Basis** | Item, Rectangle, Text, TextInput, TextEdit, Image, MouseArea, FocusScope |
| **Layout** | Column, Row, Flow, SplitView, Repeater |
| **Listen** | ListView, GridView, TableView, TreeView |
| **Modelle** | ListModel, ListElement, TableModel, TreeItemModel, GqlModel |
| **Animation** | Animation, NumberAnimation, PropertyAnimation, PauseAnimation |
| **Dialoge** | Popup, Drawer, Menu, MenuItem, MenuBar, FileDialog, FolderDialog |
| **3D** | View3D, Node3D, Model, PerspectiveCamera, DirectionalLight, PrincipledMaterial |
| **Eingabe** | DragHandler, WheelHandler, Shortcut |
| **Netzwerk** | WebSocket, WebView |
| **Karten** | Map, MapCircle, MapPolygon, MapPolyline, MapQuickItem |
| **Utility** | Timer, Settings, Component, Connections, Binding, Loader |

### 3D-Unterstützung

JQML v2 enthält 3D-Visualisierung über Three.js (MIT-Lizenz):

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

| QML-Typ | Three.js-Typ |
|---------|-------------|
| `View3D` | `WebGLRenderer` + `Scene` |
| `Node3D` | `Object3D` / `Group` |
| `Model` | `Mesh` |
| `PerspectiveCamera` | `PerspectiveCamera` |
| `DirectionalLight` | `DirectionalLight` |
| `PrincipledMaterial` | `MeshStandardMaterial` |

Mesh-Primitives: `"#Cube"`, `"#Sphere"`, `"#Cylinder"`, `"#Cone"`, `"#Plane"`, `"#Rectangle"`

glTF-Dateien (`.glb`/`.gltf`) werden asynchron über `GLTFLoader` geladen.

➡️ Details: [Tools/JQML/v2/README-3D.md](../../Tools/JQML/v2/README-3D.md)

### Neue Komponente zu JQML v2 hinzufügen

1. JavaScript-Datei in `Tools/JQML/v2/components/` erstellen
2. In `components/list.js` registrieren
3. Fertig — kein Compiler-Code nötig

```javascript
// components/MyComponent.js
class MyComponent extends Item {
    // Implementierung
}
```

### TreeItemModel API-Kompatibilität

Beim Zugriff auf TreeItemModel-Methoden müssen sowohl Groß- als auch Kleinschreibung unterstützt werden:

| Qt/C++ (Großbuchstabe) | JQML (Kleinbuchstabe) |
|------------------------|----------------------|
| `GetTreeItemModel` | `getTreeItemModel` |
| `GetItemsCount` | `getItemsCount` |
| `GetData` | `getData` |

---

## JQML v3 — JQ

### Architektur

```
Tools/JQML/v3/
├── compiler/
│   ├── compiler.js     ← Haupt-Compiler (2.133 Zeilen)
│   └── parser.js       ← QML-Parser basierend auf UglifyJS (1.935 Zeilen)
├── core/               ← Runtime-Engine
│   ├── JQApplication   ← Globaler Lifecycle
│   ├── JQContext        ← Ausführungs-Scope, Bindings
│   ├── MouseController  ← Maus-Event-Dispatch
│   ├── KeyboardController ← Tastatur-Event-Dispatch
│   ├── ImageController  ← Bild-Laden und -Caching
│   ├── TextController   ← Text-Messung
│   └── MemoryController ← Objekt-Lifecycle
├── dist/main.js        ← Webpack-Bundle (Runtime)
├── Qt/                 ← Qt-Global-Modul (Enums, Hilfsfunktionen)
├── QtBase/             ← Basis-Klassen-Infrastruktur
├── QtQml/              ← ~60 Kern-Typen
├── QtQuick/            ← ~50 UI-Komponenten
├── Qt5Compat/          ← Qt 5 Kompatibilitäts-Layer
├── QtLocation/         ← Karten-Typen
├── QtPositioning/      ← Geo-Koordinaten
├── QtWebSockets/       ← WebSocket
└── package.json
```

### Kompilierungs-Pipeline

Der v3-Compiler durchläuft 7 Phasen:

```
Phase 1: CLI-Parsing & Konfiguration
    ↓
Phase 2: Config-Datei laden (JSON mit dirs, includes)
    ↓
Phase 3: Modul-Entdeckung (qmldir-Dateien parsen)
    ↓
Phase 4: Einzeldatei-Parsing (App-Komponenten)
    ↓
Phase 5: Code-Generierung (QML → JavaScript-Klassen)
    ↓
Phase 6: Abhängigkeits-Sortierung (topologisch)
    ↓
Phase 7: Output (Bundle + Source Map)
```

### Konfiguration

Config-Datei (JSON):

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

Umgebungsvariablen (`${VAR}`) werden automatisch ersetzt.

### Build-Befehle

```bash
cd Tools/JQML/v3

# Abhängigkeiten installieren
npm install

# Runtime-Bundle bauen (Produktion)
npm run build-core

# Runtime-Bundle bauen (Entwicklung)
npm run build-core-dev

# QML-Projekt kompilieren
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

### HTML-Modus

Der Compiler kann auch eine Boilerplate-HTML-Seite erzeugen:

```bash
npm run compile -- -c config.json -e MainApp.qml -o output -n myapp -m html -i icon.png
```

### Eingebaute Module

| Modul | Inhalt |
|-------|--------|
| `Qt` | Enums, `Qt.rgba()`, `Qt.lighter()`, `Qt.binding()` |
| `QtQml` | Component, Connections, Binding, Timer, Property-Typen |
| `QtQuick` | Item, Rectangle, Text, Image, MouseArea, Animationen, Layouts |
| `Qt5Compat` | Qt 5 Kompatibilitäts-Shims |
| `QtWebSockets` | WebSocket |
| `QtPositioning` | GeoCoordinate |
| `QtLocation` | Map, MapCircle, MapPolygon, MapPolyline |

### QMake-Integration

Der JQML-Compiler wird automatisch während des Qt-Builds über `Config/QMake/WebCompiler.pri` ausgeführt:

- Verwendet `QMAKE_EXTRA_COMPILERS` für einen benutzerdefinierten Build-Schritt
- Kompiliert bei Änderungen an QML-Dateien
- Erzeugt eine `.qrc`-Datei, die in die Anwendung eingebettet wird

---

## Cross-Platform-Entwicklung

### Prinzipien

1. **Ein QML-Quellcode** für Desktop und Web
2. **Module über qmldir** — der Compiler findet Komponenten automatisch
3. **Keine plattformspezifischen Imports** im Geschäftslogik-Code

### Plattformspezifischer Code

Für Fälle, in denen Desktop und Web unterschiedliches Verhalten benötigen:

```
Qml/imtcontrols/Base/         ← Standard (Desktop)
Qml/imtcontrols/Base/web/     ← Web-Überschreibungen
```

Web-spezifische Dateien in `web/`-Unterordnern überschreiben ihre Desktop-Pendants:
- `ScrollBar.qml` → Web-kompatibler Scrollbalken
- `ToolTip.qml` → Web-kompatibler Tooltip
- `Switch.qml` → Web-kompatibler Schalter
- `Window.qml` → Web-kompatibler Fenster-Wrapper

### QML-Regeln für JQML-Kompatibilität

#### ✅ Machen

- Standard-QML-Properties und deklarative Bindings verwenden
- `ListModel` für statische Daten
- Data-Provider-Muster für dynamische Daten
- `import imtcontrols 1.0` statt direkter Qt-Imports wo möglich

#### ❌ Vermeiden

- **`prop: { ... JS-Block ... }`** für Model-Bindings
  ```qml
  // ❌ FALSCH
  // model: { return computeModel() }

  // ✅ RICHTIG
  model: myDataProvider.items
  ```

- Desktop-spezifische Qt-Typen ohne JQML-Äquivalent
- Direkte C++-Aufrufe ohne QML-Wrapper-Schicht
- `Qt.createComponent()` in Performance-kritischem Code

#### TreeItemModel-Zugriff

Sowohl Groß- als auch Kleinschreibung prüfen:

```qml
// In TreeModelBuilder
function childModel(item) {
    if (typeof item.GetTreeItemModel === "function")
        return item.GetTreeItemModel()
    if (typeof item.getTreeItemModel === "function")
        return item.getTreeItemModel()
}
```

### Abhängigkeiten (v3)

| Paket | Version | Zweck |
|-------|---------|-------|
| `commander` | ^14.0.1 | CLI-Argument-Parsing |
| `source-map-generator` | ^0.8.0 | Source-Map-Generierung |
| `uglify-js` | ^3.19.3 | JS AST (Basis für QML-Parser) |
| `ol` | ^7.5.2 | OpenLayers (QtLocation Runtime) |
| `webpack-cli` | ^5.1.4 | Runtime-Bundling (Dev-Abhängigkeit) |

---

## Debugging

### Source Maps

JQML v3 erzeugt Source Maps (`.js.map`), die Browser-DevTools erlauben, direkt im QML-Quellcode zu debuggen.

### Render-Strategie

`View3D` (v2 3D) verwendet **On-Demand-Rendering**: Nur Property-Änderungen lösen ein `requestAnimationFrame` aus. Idle-Szenen verbrauchen keine CPU/GPU.

### Picking

`View3D` bietet:
- `pick(x, y)` — nächstes `Node3D` unter 2D-Koordinate
- `mapFrom3DScene({x, y, z})` — Welt → Bildschirm
- `mapTo3DScene({x, y, z})` — Bildschirm → Welt

---

## Weiterführende Dokumentation

- **[Tools/JQML/v2/README-3D.md](../../Tools/JQML/v2/README-3D.md)** — JQML2 3D-Dokumentation
- **[Tools/JQML/v3/compiler/README.md](../../Tools/JQML/v3/compiler/README.md)** — JQML v3 Compiler-Architektur
- **[QML Documentation Index](QML_Documentation_Index.md)** — Dokumentationsübersicht
- **[QML Module Reference](QML_Module_Reference.md)** — Alle QML-Komponenten
- **[QML Getting Started](QML_Getting_Started.md)** — Einstiegstutorial
- **[QML Component Tutorials](QML_Component_Tutorials.md)** — Praktische Tutorials
