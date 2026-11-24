# JQML 4 Composer

The JQML 4 Composer is a VS Code extension of its own, in `Tools/JQML/v4/plugins/jqml-composer`. It is not a part of the JQML 4
language extension ([VSCodeExtensionV4.md](VSCodeExtensionV4.md), `Tools/JQML/v4/plugins/jqml-vscode`); the two can be installed
together or one without the other.

## Schemas and generation

The **Composer** is a visual editor of *schemas*, made like the composer of AcfSln: a palette of components, the tree
of a composition, a canvas and an inspector of attributes. It does not edit QML files. A schema is a document
(`*.jqschema`, JSON) that describes a composition; the application is **generated** from schemas.

- **New schema**: **JQML 4 Composer: New Schema** (Explorer context menu of a folder) creates `<Name>.jqschema`. The file opens
  in the Composer; **JQML 4 Composer: Open Schema as Text** shows its JSON.
- **Components**: the types of the JQML 4 engine, the types of the modules that the schema imports (`qmldir`
  modules, the compiler config of `jqml4composer.configPath`, import paths) and the other schemas of the workspace. Drag a
  component onto the canvas or onto an object of the tree (the drop goes to the nearest object that can take it),
  or double-click it. `Imports` in the toolbar are the modules the generated QML imports.
- **Inheritance**: the toolbar's **Extends** sets the schema that this one inherits. The objects of the base are
  shown (dotted, read-only) and the schema may add its own objects after them.
- **Schemas as components**: another schema can be dropped into a schema as an object (an *instance*). Its inside is
  closed.
- **Exports**: **E** next to a property of an object (or **Export** in the inspector) exports it under a name. An
  inheriting schema, and an instance of the schema, can set **only the exported properties** (the inspector lists
  them) and the properties of the placement (`x`, `y`, `width`, `height`, `z`, `visible`, `opacity`, `rotation`,
  `scale`, `clip`, `enabled`, `anchors.*`, `Layout.*`). Everything else of the base is changed in the base. Exports
  of the bases are inherited. An object that is deleted takes its exports with it.
- **Canvas**: the objects are drawn from the numbers in `x`, `y`, `width`, `height`, `anchors.*`, `radius`, `color`,
  `text` and `font.pixelSize`; the values that exports get in the inheriting schema or the instance are applied, so
  the canvas shows what is generated. `Column` and `Row` put their children one after another. An object can be
  moved and resized with the mouse when its place is made of plain numbers. This is an outline of the structure,
  not a rendering by the engine.
- **Generate**: **Generate** (or **JQML 4 Composer: Generate Application from Schema**) writes the application and compiles
  it with the JQML 4 compiler (`compiler/compiler.js`) to a folder you choose:
  `src/<Type>.qml` for every schema that is needed, `constructor.config.json`, and `app/<name>.js` with
  `app/<name>.html`. The sources are written even when no engine is found.

How a schema becomes QML: every schema is a QML type named after its file (`MainPage.jqschema` is `MainPage`), so
the files must be in one folder after generation and their names must differ. A schema that extends another is
`Base { ... }`; an instance is an object of its type; an export is `property alias name: id.property` of the root;
the objects get an `id` (their own, or one made from the type). Values of the properties are QML expressions
(`"text"`, `10`, `parent.width`, a handler body).

```json
{
  "version": 1,
  "imports": ["QtQuick 2.12"],
  "extends": "Card.jqschema",
  "root": { "uid": "br", "type": "", "id": "big", "properties": { "title": "\"Big\"", "width": "320" }, "children": [] },
  "exports": [ { "name": "badgeColor", "uid": "x", "property": "color", "type": "color" } ]
}
```

Generation stops with the list of the problems when a schema sets a property that the base does not export, when an
export points to an object that is gone, when a schema file is missing, or when schemas extend each other in a cycle;
the same problems are shown at the top of the Composer. While the JSON of a schema is broken the Composer is
read-only. Renaming an export does not rename it in the schemas that use it.

## Settings

The Composer has its own settings, so it needs nothing of the language extension. They are the same as the ones that the language
extension has for the project:

- `jqml4composer.configPath`: a JQML compiler JSON config with dirs/includes (the modules of the project and the compile of the
  generated application). `${workspaceFolder}` and `${env:NAME}` are supported.
- `jqml4composer.env`: the variables for the `${NAME}` placeholders of that config.
- `jqml4composer.importPaths`: folders searched for `<Module>/qmldir`.
- `jqml4composer.enginePath`: the root of the JQML v4 sources (the folder with `src/items.js` and `compiler/compiler.js`). Empty: the
  folder the extension was built from.

## How it is made

The palette and the inspector need the types of the engine and of the imported modules and their properties. The Composer runs a copy
of the JQML 4 language service for that (`language/`). `npm run build` prepares it: it runs the build of `jqml-vscode`, which makes
`lib/` (the JQML v4 lexer, parser, config reader and `engine-index.json`), and copies that `lib/` and `service.mjs`, `project.mjs`,
`scanner.mjs` into `language/`. Nothing in `jqml-vscode` is changed by it, and `language/` is not committed.

## Install

```
cd Tools/JQML/v4/plugins/jqml-composer
npm run package
```

Then in VS Code: **Extensions: Install from VSIX...** and choose `jqml4-composer.vsix`.

## Tests

```
cd Tools/JQML/v4/plugins/jqml-composer
npm test
```
