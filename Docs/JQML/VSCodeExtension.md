# JQML VS Code extension

The JQML language extension lives in `Tools/JQML/v3/vscode-extension`.

## Install

The extension is installed as a VSIX, not launched from source:

```
npm run package-extension
```

Then in VS Code: **Extensions: Install from VSIX...** and choose `Tools/JQML/v3/vscode-extension/jqml-1.0.4.vsix`.

Or from a terminal:

```
code --install-extension Tools/JQML/v3/vscode-extension/jqml-1.0.4.vsix
```

The package includes the parser and a snapshot of engine component types, so a live JQML tree is not required after install.

## Settings

- `jqml.configPath` — compiler JSON with `dirs`/`includes` (for example `ProLife/Qml/Include/prolifeqml/prolife.json`). Absolute paths, `${workspaceFolder}`, and `${env:NAME}` are supported.
- `jqml.env` — optional extra variables for placeholders such as `${IMTCOREDIR}`. If `IMTCOREDIR`, `PROLIFEDIR`, and `TARGETNAME` are missing from the environment, the language service infers them from the config path and workspace folders.
- `jqml.enginePath` — optional live JQML engine root. Empty uses the types bundled in the VSIX.
- `jqml.hideInternalMembers` — hide `JQ*` / `AX` members from completions. Names starting with `__` are always hidden.

Ctrl+click (Go to Definition) on a property, method, or signal opens its QML declaration, or the engine/QML file it was inherited from.

Completions follow the cursor context: `import`/`pragma`/root types outside the first element; properties, signals, handlers, functions, inherited members and child types inside an element; members and ids inside a function or slot; members of that object after a `.`.

After changing settings, reload the window or re-open a `.qml` file.

## Debug from source

From `Tools/JQML/v3`, run the **Launch JQML Extension** configuration. Tests: `npm run test-language`.
