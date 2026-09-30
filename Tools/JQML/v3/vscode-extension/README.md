# JQML for VS Code

Language support for the JQML engine: syntax highlighting, completions, and diagnostics.

## Install from VSIX

Build the package once:

```
npm run package-extension
```

This creates `vscode-extension/jqml-1.0.4.vsix`. Then install it into VS Code in any of these ways:

- Command Palette: **Extensions: Install from VSIX...** and pick the `.vsix` file
- CLI: `code --install-extension Tools/JQML/v3/vscode-extension/jqml-1.0.4.vsix`

After that the extension is a normal installed extension. It starts itself when you open a `.qml` file. Restart VS Code if it does not appear immediately.

If another QML extension already claims `.qml`, set:

```json
"files.associations": {
    "*.qml": "jqml"
}
```

Ctrl+click a property, method, or signal to jump to its declaration (QML or inherited engine file). Names starting with `__` are not offered in completions.

## Settings

| Setting | Purpose |
| --- | --- |
| `jqml.enginePath` | Optional live engine root. Empty = bundled engine types. |
| `jqml.configPath` | Compiler JSON (`dirs` / `includes`). Only this path is loaded. `${workspaceFolder}` and `${env:NAME}` work in the setting. |
| `jqml.env` | Environment values for `${NAME}` placeholders in the config path and in the JSON. Overrides the process environment. |
| `jqml.hideInternalMembers` | Hide `JQ*` and geometry internals from completions. Names starting with `__` are always hidden. |
