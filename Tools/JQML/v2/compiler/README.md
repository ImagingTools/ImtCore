# JQML v2 Compiler

Node.js implementation of the JQML v2 QML-to-JavaScript compiler.

## Overview

The compiler transforms `.qml` source files into a single bundled JavaScript
file (`jqml.full.js`) that can be loaded in a web browser. This allows ImtCore
Qt/QML applications to be deployed as web clients without rewriting the UI
layer.

```
Qml/imtgui/*.qml          ┐
Qml/imtcontrols/*.qml     ├─► preparesources.py ─► build/src/ ─► compiler.js ─► jqml.full.js
SDL-generated/*.qml        ┘                                         (this dir)
```

## File Structure

```
compiler/
├── compiler.js          Entry point — accepts <source-dir> as argument
├── package.json         Node.js project metadata
├── src/
│   ├── lexer.js         QML tokenizer
│   ├── parser.js        Recursive-descent QML parser → AST
│   ├── codegen.js       AST → JavaScript code generator
│   └── bundler.js       Directory walker + JQML runtime header
└── test/
    └── run.js           Self-contained test suite
```

## Requirements

| Tool    | Minimum version |
|---------|-----------------|
| Node.js | 14.0.0          |

No npm dependencies — the compiler is plain Node.js.

## Usage

```bash
node compiler.js <source-dir>
```

`<source-dir>` is the directory that has been populated by
`preparesources.py`. The compiler walks it recursively, processes every `.qml`
and `.js` file it finds, and writes the bundle to
`<source-dir>/jqml.full.js`.

### Example

```bash
node compiler.js /build/myapp/web/src
# Output: /build/myapp/web/src/jqml.full.js
```

### Verbose output

```bash
JQML_VERBOSE=1 node compiler.js /build/myapp/web/src
```

### Exit codes

| Code | Meaning                                              |
|------|------------------------------------------------------|
| 0    | All files compiled successfully                      |
| 1    | One or more files had errors (bundle still written)  |
| 2    | Fatal error (bad arguments, I/O failure)             |

## Generated output format

Each `.qml` file produces a `JQML.define()` call:

```js
// Generated from MyButton.qml
JQML.define("MyButton", {
    base: "Rectangle",
    id:   "root",
    properties: {
        "label":   { meta: { type: "string", alias: false, readonly: false }, value: "OK" },
        "enabled": { meta: { type: "bool",   alias: false, readonly: false }, value: true  },
    },
    signals:  [{ name: "clicked", params: [] }],
    handlers: {
        "onClicked":            function() { doAction(); },
        "Component.onCompleted": function() { initialize(); },
    },
    bindings: {
        "width":  function() { return parent.width; },
    },
    functions: {
        doAction: function() { /* ... */ },
    },
    children: [
        {
            type: "Text",
            id:   "label",
            bindings: { "text": function() { return root.label; } },
            ...
        }
    ]
});
```

The bundle is prefixed with the **JQML v2 runtime** which exposes:

```js
JQML.define(name, descriptor)  // Register a component
JQML.create(name, overrides)   // Instantiate a component
JQML.get(name)                 // Look up a descriptor
JQML.list()                    // List all registered names
```

Instances returned by `JQML.create()` expose:

| Method                  | Description                                        |
|-------------------------|----------------------------------------------------|
| `.get(propName)`        | Read a property (resolves binding functions)       |
| `.set(propName, value)` | Write a property, fires `onPropNameChanged` if set |
| `.emit(signalName)`     | Emit a signal, invokes matching `onXxx` handler    |
| `.call(fnName, ...args)`| Call a QML function                                |
| `.complete()`           | Fire `Component.onCompleted`                       |

## Running the tests

```bash
node test/run.js
```

Expected output:

```
Lexer
  ✓  import keyword recognised
  ✓  line comment skipped, property recognised
  ...
Parser
  ✓  import parsed
  ✓  component type
  ...
Code generation
  ✓  JQML.define call emitted
  ...
28 tests: 28 passed, 0 failed
```

## Parser coverage

| QML construct                      | Supported |
|------------------------------------|-----------|
| `import Module Version`            | ✅        |
| `import "path" as Alias`           | ✅        |
| Root component `TypeName { }`      | ✅        |
| `id: identifier`                   | ✅        |
| `property type name`               | ✅        |
| `property type name: value`        | ✅        |
| `property alias name: id.prop`     | ✅        |
| `readonly property ...`            | ✅        |
| `default property ...`             | ✅        |
| `signal name()`                    | ✅        |
| `signal name(type param, ...)`     | ✅        |
| `onSignal: expression`             | ✅        |
| `onSignal: { block }`              | ✅        |
| `Object.onSignal: { block }`       | ✅        |
| `function name(params) { block }`  | ✅        |
| Property binding `name: expr`      | ✅        |
| Dotted binding `a.b: expr`         | ✅        |
| Nested child components            | ✅        |
| Multi-line JS function bodies      | ✅        |

## Limitations

- The parser captures JavaScript function/handler bodies as raw strings — it
  does not fully parse the embedded JavaScript.
- Multi-line binding expressions (ternary across lines, etc.) may be
  truncated; wrap them in `{ return ...; }` to be safe.
- QML `Connections {}`, `Repeater {}`, and other dynamic list elements are
  parsed as generic child components.
- `pragma` directives are silently skipped.
