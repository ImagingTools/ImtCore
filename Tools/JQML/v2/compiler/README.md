# JQML2 – QML-to-Web Compiler

This directory contains the **QML-to-JavaScript transpiler** for the JQML2 library.
It converts Qt Meta Language (QML) source files into a single bundled JavaScript file
(`jqml.full.js`) that can be executed in a web browser.

## Files

| File | Lines | Purpose |
|------|-------|---------|
| `compiler.js` | 1,565 | Main transpilation engine (orchestration, semantic analysis, code generation) |
| `parser.js` | 1,920 | QML/JavaScript lexer and parser based on UglifyJS / QmlWeb |

---

## Architecture Overview

The compiler works as a **multi-stage transpiler**:

```
QML source files (*.qml)
        │
        ▼ 1. Discovery   getFiles()
File path list
        │
        ▼ 2. Parsing     parser.parse()
Abstract Syntax Trees (AST)
        │
        ▼ 3. Metadata extraction   preCompile()
Instruction objects
(properties, signals, methods, children, imports)
        │
        ▼ 4. Semantic analysis   prepare()
Resolved property bindings / JavaScript expression strings
        │
        ▼ 5. Code generation   treeCompile()
JavaScript class definitions (ES6)
        │
        ▼ 6. Output   fs.writeFileSync()
jqml.full.js
(Enums JSON + class definitions + singleton registry)
```

---

## Stage 1 – Discovery (`getFiles`)

`getFiles(dir)` recursively walks a source directory and collects all files
with the `.qml` extension.  The root path is taken from the command-line
argument `source` (or a hardcoded fallback).

Each path is normalised to forward slashes, and a parallel `classList` array
records the **class name** derived from the relative path (relative to the
source root) by replacing path separators and the `.qml` suffix with
underscores:

```
<source-root>/controls/Button.qml  →  controls_Button
```

---

## Stage 2 – Parsing (`parser.js`)

`parser.js` is a lightly modified build of the **UglifyJS** tokenizer/parser
(BSD-2-Clause, © Mihai Bazon) extended by the **QmlWeb** project to understand
QML-specific syntax.

### Tokenizer

Recognises:

- Standard JavaScript keywords (`break`, `case`, `const`, …)
- QML-only keywords: `import`, `pragma`, `signal`, `property`, `alias`, `readonly`
- Operators, punctuation, number and string literals, regular expressions

### Parser / AST

Produces an S-expression-style AST.  Every node is a plain JavaScript array
whose first element is a string tag, e.g.:

| Tag | Meaning |
|-----|---------|
| `toplevel` | Whole QML document |
| `qmlpragma` | `pragma Singleton` etc. |
| `qmlimport` | `import QtQuick 2.15` |
| `qmlelem` | A QML element block `Rectangle { … }` |
| `qmlpropdef` | `property int count: 0` |
| `qmlaliasdef` | `property alias text: label.text` |
| `qmlsignaldef` | `signal clicked(int x, int y)` |
| `qmlmethod` | `function doSomething() { … }` |
| `qmlprop` | Property assignment `width: 100` |
| `qmlenumdef` | `enum Status { Active, Inactive }` |
| `qmlobj` | Inline `QtObject { … }` |
| `name`, `dot`, `call`, `assign`, … | Standard JavaScript expression nodes |

The exported surface is:

```js
module.exports.parse   = qmlweb_parse;   // QML document → AST
module.exports.jsparse = qmlweb_jsparse; // JS resource  → exports list
```

---

## Stage 3 – Metadata Extraction (`preCompile`)

`preCompile(className, meta, on, instructions)` walks one level of the AST and
populates an **instruction object** created by `getBaseStructure()`.

### Instruction Object Structure

```js
{
    className:        string,          // resolved class name (component or QML file)
    extends:          compiledFile|null,
    Singleton:        boolean,
    fileName:         string,
    id:               string,          // QML id: "myRect"
    properties:       Array<PropDef>,  // all property definitions/assignments
    propertiesNames:  string[],
    methods:          { [name]: MethodDef },
    defineSignals:    { [name]: string[] }, // signal name → parameter list
    connectionSignals: Array<SignalConn>,   // onXxx handlers
    children:         Instruction[],
    parent:           Instruction|null,
    name:             string,          // generated variable name "el1", "el2", …
    UID:              number,
    compiledFile:     CompiledFile,
}
```

### Handler Functions

| Function | QML Construct | Action |
|----------|--------------|--------|
| `qmlpragma` | `pragma Singleton` | Sets `Singleton = true`, registers name in `SingletonList` |
| `qmlimport` | `import …` | Adds to `compiledFile.imports` |
| `qmlelem` | Nested element `Foo { }` | Recurses via `preCompile`, appends to `children` |
| `qmlpropdef` | `property type name: val` | Appends typed property to `properties` with `command: 'create'` |
| `qmlaliasdef` | `property alias …` | Appends `QAlias` property |
| `qmlsignaldef` | `signal name(…)` | Adds to `defineSignals` |
| `qmlmethod` | `function name(…)` | Adds to `methods` |
| `qmlprop` | `name: value` or `onSignal: …` | Routes to `properties` or `connectionSignals` |
| `qmlobj` | Inline `QtObject` | Creates a child instruction of type `QtObject` |
| `qmlenumdef` | `enum Name { … }` | Stores in global `Enums` map |

### Class Name Resolution

When the element type is not a built-in component, `preCompile` resolves the
class name by:

1. Checking `namespace_ClassName` in the known class list.
2. Searching the file's import list for a matching path prefix.
3. Falling back to the shortest unambiguous name from `classList`.

---

## Stage 4 – Semantic Analysis (`prepare`)

`prepare(tree, compiledFile, currentInstructions, stat, …)` walks an
expression/statement sub-tree and writes JavaScript source fragments into
`stat.value[]`.

It also determines whether an expression is **reactive** (`stat.compute = true`),
meaning it references component properties or context variables that can change
at runtime.

### Name Resolution in `prepare`

| Source token | Generated JavaScript |
|-------------|---------------------|
| `parent` | `elN.parent` |
| QML `id` in same file | `inCtx.get('myId')` |
| Known property/signal/method on current element | `elN.propName` |
| Singleton (via `pragma Singleton`) | `inCtx.get('SingletonName')` |
| `Qt.*` member | `Qt.memberName` |
| Global JS identifier | verbatim |

### Supported AST Nodes

All standard JavaScript constructs are handled:
`return`, `num`, `string`, `assign`, `name`, `dot`, `call`, `conditional`,
`binary`, `let`/`var`, `unary-prefix`, `unary-postfix`, `if`, `while`,
`for`, `for-of`, `for-in`, `function`, `defun`, `object`, `block`, `stat`,
`new`, `sub`, `array`, `regexp`, `switch`, `seq`, `try`, `break`,
`continue`.

`QAlias` properties receive special treatment: path segments are translated to
`getStatement('segment')` chains for lazy property lookup.

---

## Stage 5 – Code Generation (`treeCompile`)

`treeCompile(compiledFile, instructions, updatePrimaryList, updateList, step, innerComponent)`
is the recursive code emitter.  It writes lines into the global `code[]`
array.

### Per-element steps

1. **Instantiate** – `let elN = new ClassName(parent, inCtx)` (or `let el1 = this` for the root).
2. **Register id** – `inCtx.add('id', elN)` if the element has an `id`.
3. **Anchor / font properties** – processed first so layout is computed before
   other reactive properties.
4. **Property bindings** – for each property:
   - *Static value*: `elN.prop = value`
   - *Reactive (compute) value*: `elN.prop.setCompute(function(){ return … })`
   - *New property with type*: `elN.createProperty('name', QType, …)`
   - *Variant property*: `elN.createVariantProperty('name', type, …)`
5. **Signal handlers (early)** – `onChanged` handlers for signals that are
   *not yet defined* as custom signals are connected via `getNotify().connect(…)`.
6. **Methods** – `elN.methodName = function(…){ … }`, wrapped in
   `TransactionController.begin/end`.
7. **Signal handlers (main pass)** – remaining `onXxx` handlers connected via
   `getStatement('signal').connect(…)`.
8. **Custom signals** – `elN.createSignal('name', …param names…)`.
9. **Children** – recursive `treeCompile` call for each child element.
10. **Update list** – at `step === 0` (root), emits
    `updateList.push(()=>{ anchors.update(); … })` to trigger initial reactive
    evaluation after construction.

### Special elements

| Component | Extra handling |
|-----------|----------------|
| `Component` | `createObject` function is generated inline instead of child iteration |
| `Loader` | `$widthAuto` / `$heightAuto` flags set based on anchor configuration |
| `Repeater`, `ListView`, `GridView`, `MapItemView` | `delegate` property wrapped in a `Component.createObject` factory |

---

## Stage 6 – Class Wrapper and Output

After all instruction trees are compiled, `compiler.js` wraps each in an ES6
`class … extends …` declaration and emits a `constructor`:

```js
class controls_Button extends Rectangle {
    constructor(parent, exCtx, exModel) {
        let inCtx = new ContextController(exCtx)
        super(parent, inCtx, exModel)
        // … treeCompile output …
    }
}
classList['controls_Button'] = controls_Button
```

For **Singleton** files the constructor additionally:
- Returns the cached instance if one exists.
- Stores the new instance in `Singletons['$Name']`.
- If the singleton is named `Enums`, copies all enum keys onto `this`.

The final file `jqml.full.js` has the structure:

```
Enums = { EnumType: { Key: value, … }, … }
<class definitions in dependency order>
SingletonClass['Name'] = ClassName      // for each Singleton
classList['ClassName'] = ClassName      // for every class
```

---

## Runtime Dependencies

The generated code relies on the following runtime modules bundled separately
by Webpack (`webpack.config.js`):

| Module | Location | Role |
|--------|----------|------|
| Component base classes | `utils/base.js` | Base class mixins |
| Qt API | `utils/Qt.js` | `Qt.*` namespace, colour helpers, etc. |
| Property types | `utils/properties.js` | `QInt`, `QDouble`, `QBool`, `QString`, … |
| Signal/slot | `utils/signal.js` | Signal connection and emission |
| Controllers | `utils/controllers.js` | `TransactionController`, `ContextController` |
| Built-in components | `components/*.js` | `Rectangle`, `Text`, `MouseArea`, … (40+ components) |

---

## Usage

```bash
# Compile QML sources from the default source directory
npm run compile

# Compile from an explicit directory (relative to compiler.js)
node ./compiler/compiler.js path/to/qml/src

# Compile from the test fixtures directory
npm run compile-test

# Enable verbose error mode (throws on first warning)
node ./compiler/compiler.js source=path debug=1
```

The output file `jqml.full.js` is written into the source directory passed on
the command line.

---

## Known Limitations / Notes

- Comments (`// …` and `/* … */`) are stripped from QML source before parsing
  via a regular expression; this is not a full comment parser and may misbehave
  inside string literals containing comment-like sequences.
- Class dependency ordering relies on a simple queue that defers a file when its
  parent class has not yet been emitted.  Circular dependencies are not detected
  and will cause an infinite loop.
- Some identifiers (`Gql`, `Icon`, `modalDialogManager`) are hard-coded as
  suppressed warnings, reflecting project-specific globals not visible to the
  compiler.
- The `prepare()` function falls back to `eval(identifier)` to distinguish
  known JavaScript globals from unknown QML names; this is a heuristic and may
  produce incorrect results for unusual identifiers.
