# JQML versions in the web build

`Tools/JQML` holds the web compilers. Products that build their web client with `jq_compile_web`
(`Config/CMake/WebCompiler.cmake`, products with `USE_NEW_WEB`) use one of three versions:

| Version | Folder | Notes |
|---|---|---|
| v3 | `Tools/JQML/v3` | default |
| v4 | `Tools/JQML/v4` | |
| v5 | `Tools/JQML/v5` | snapshot of the JQMLNext repository |

## Choosing the version

The version that the product serves at `<ProductName>/Views/html/index.html` is the first of:

1. the CMake cache variable `JQML_VERSION` (`v3`, `v4`, `v5`; empty by default);
2. the environment variable `JQML_VERSION`;
3. the product's own default, `jqmlversion`, set before `jq_compile_web`;
4. `v3`.

```
cmake -DJQML_VERSION=v4 <build dir>    # this build only
cmake -DJQML_VERSION= <build dir>      # back to the product's default
```

A product picks its default in the CMake file that calls `jq_compile_web`, for example ProLife:

```
set(jqmlversion "v5")
```

The configure log names the result: `JQML v5 for prolifeqml`.

## Building all versions

When all versions are on, the other two versions are built too and put in the same resource. The switch is
the first of: the CMake cache variable `JQML_WEB_ALL_VERSIONS` (`ON`, `OFF`; empty by default), the
environment variable `JQML_WEB_ALL_VERSIONS`, the product's `jqmlallversions`, `OFF`. ProLife sets
`jqmlallversions` to `ON`.

| Address | Version |
|---|---|
| `<ProductName>/Views/html/index.html` | the selected version |
| `<ProductName>/Views/htmlN/index.html` | every other version N, for example `html4`, `html5` |

```
cmake -DJQML_WEB_ALL_VERSIONS=ON <build dir>
```

Switching between the versions in the browser means changing `html` in the address. The versions share the
origin, so they share the session, cookies and local storage. Fonts, icons and translations are shared.

The web build takes about three times as long and the executable grows by the two extra bundles, so it is
off unless the product or the build turns it on.

Each extra version is compiled into `<buildwebdir>/Resources/vN` with its own compiler. Its resource file
is the version's `dist/qmlJsWeb.qrc` with the prefix `/html` replaced by `/htmlN`.

## Updating v5

v5 is a copy of the JQMLNext repository, which is where it is developed and tested. To update it, copy
these from a JQMLNext commit, unchanged:

- `compiler/`, `Runtime/`, `dist/` (`dist/core.js` is the built runtime, `npm run build` in JQMLNext);
- `preparesources.py`, `generate_translations_qrc.py`.

`node_modules` holds only `uglify-js`, the same version as in v3. `.gitignore` excludes `node_modules`,
`Bin/` and `Lib/` everywhere except under `Tools/JQML/v5/node_modules`, so a plain `git add` takes it whole.

## What v5 needs to build

Nothing is installed during the build; everything v5 needs is in the repository:

- Node.js 12 or newer. On Windows it is `3rdParty/nodejs/node.exe` (a submodule, the executable is in Git LFS);
  elsewhere `node` from the `PATH`, or `NODE_EXE`. The configure step stops with a message if Node.js does not
  run or is too old.
- Python 3.8 or newer for `preparesources.py`, as for v4.
- `dist/core.js` is built in JQMLNext and committed; webpack is not needed.
