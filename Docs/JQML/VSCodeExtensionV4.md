# JQML 4 VS Code extension

The JQML 4 language extension lives in `Tools/JQML/v4/plugins/jqml-vscode`. `Tools/JQML/v4/plugins` is the
folder for the tools that are built around JQML 4 (the next plugins go next to it). The extension for JQML 3 is
described in [VSCodeExtension.md](VSCodeExtension.md).

It uses the real JQML 4 lexer and parser (`v4/src/lexer.js`, `v4/src/parser.js`) for syntax errors, and the list
of the built-in types and their members is read from the engine classes (`v4/src/items.js`) when the extension
is built. The extension needs no running JQML instance.

## Features

- **Diagnostics while typing**: syntax errors of the structure and of the script fragments, duplicate `id`s,
  types that are not found, properties and signal handlers that the type does not have (a name that looks like a
  typo of a known one is a warning with `Did you mean`, other names are information), imports that are not found.
- **Completion**: root types and `import` outside the first element; properties, signals, handlers, keywords and
  child types inside an element; ids, `parent`, members and singletons in a value or a script; members after a
  `.` (`card.`, `parent.`, `anchors.`, `Theme.`, `Qt.`). It works while the file has a syntax error.
- **Go to Definition** (Ctrl+click): a type opens its `.qml` file, a property, signal, function or handler opens
  the declaration in the file or the base type that declares it, an id opens its declaration, `import` opens the
  `qmldir`. A member of a built-in type opens its class in `v4/src/items.js` when the sources are known.
- **Hover**, **Find All References** (by name inside the file) and the **Outline** of a file.

Types are found as the engine finds them: the later `import` wins over an earlier one, every import wins over
the folder of the file, `import "folder"` and `import Module 1.0 as Alias` are supported, and the newest version
that an import allows is taken (`Project` in `v4/src/project.js`).

## Install

```
cd Tools/JQML/v4/plugins/jqml-vscode
npm run package
```

Then in VS Code: **Extensions: Install from VSIX...** and choose `jqml4.vsix`, or

```
code --install-extension Tools/JQML/v4/plugins/jqml-vscode/jqml4.vsix
```

The extension registers the language `jqml4` for `*.qml`. The JQML 3 extension also registers `*.qml`, so
install only one of them in a VS Code profile, or set the language of a workspace with
`"files.associations": { "*.qml": "jqml4" }`.

## Settings

- `jqml4.configPath` — the compiler JSON with `dirs`/`includes` (for example `Qml/web/imtcore.json`).
  `${workspaceFolder}` and `${env:NAME}` are supported. Included configs that do not exist (generated files that
  are not built yet) are skipped and listed in the **JQML 4** output channel.
- `jqml4.env` — variables for `${NAME}` placeholders in the config. `IMTCOREDIR` is taken from the process
  environment or, when it is missing, from the closest parent folder of the config that holds `Qml/imtcontrols`.
- `jqml4.importPaths` — folders searched for `<Module>/qmldir`.
- `jqml4.enginePath` — the folder with `src/items.js`, used to open the source of built-in types. Empty: the
  folder the extension is run from, when it is the source tree.
- `jqml4.hideInternalMembers` — hide names that start with `_` from completions.
- `jqml4.diagnostics.unknownTypes`, `jqml4.diagnostics.unknownMembers` — switch the two semantic checks.

Every `qmldir` in the workspace is indexed too, so a module of the workspace is found without a config.
The command **JQML 4: Reload project** reads the settings and the `qmldir` files again.

A module that no config or import path knows (`com.imtcore.imtqml`, generated SDL modules) is reported once per
import as information, and the unknown-type check is skipped in that file, because its types are not known.

## Develop

```
npm run build   # lib/: copy of the lexer, parser and config reader, engine-index.json
npm test        # build, then the tests of the language service
```

`lib/` is generated and ignored by git. Run `npm run build` after the engine's built-in types change, so the
index follows them. To debug, run `npm run build`, open `Tools/JQML/v4/plugins/jqml-vscode` in VS Code and start an Extension Development Host (`code --extensionDevelopmentPath=<that folder>`).

| File | Purpose |
|---|---|
| `scripts/build.mjs` | copies the lexer/parser/config and writes `lib/engine-index.json` |
| `src/scanner.mjs` | tolerant token scan: objects, ids, declarations, the statement under the cursor |
| `src/project.mjs` | `qmldir` modules, config, imports, type lookup |
| `src/service.mjs` | diagnostics, completion, hover, definition, references, outline |
| `src/extension.js` | the VS Code wiring |
