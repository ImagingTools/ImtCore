# Editor-only QML metadata

[Русская версия](README.ru.md)

`qml_tooling_metadata` describes procedurally registered C++ QML types for
`qmlls` and `qmllint`. It belongs to ImtCore and is defined in
[Config/QmlTooling](../../Config/QmlTooling/CMakeLists.txt). No product depends
on it. It is not an `ALL` target, does not compile or link C++, and has no
install or deployment rules. Runtime registration and application import
paths remain unchanged.

## Generate

The commands in this section run from the **ImtCore repository root**.
Python 3.8 or newer and a host Qt 6 installation providing `moc`,
`qmltyperegistrar` and metatypes JSON are required. Use the same Qt installation
as the editor's QML tools.

The target is available after reconfiguring an existing ImtCore build:

```sh
cmake --build <imtcore-build-directory> --target qml_tooling_metadata
```

It can also be configured independently, without configuring any library or product:

```sh
cmake -S Config/QmlTooling -B out/qml-tooling-build \
    -DIMT_QML_TOOLING_QT_ROOT=/path/to/Qt/6.x/host-kit \
    -DIMT_QML_TOOLING_OUTPUT="$PWD/out/qml-tooling"
cmake --build out/qml-tooling-build --target qml_tooling_metadata
```

Or run the generator directly, without CMake:

```sh
python3 Config/QmlTooling/generate.py \
    --qt-root /path/to/Qt/6.x/host-kit --output out/qml-tooling
```

The generator locates the ImtCore source root from its own path, not the
working directory. `--source-root` and `--manifest` can override the defaults.

The CMake cache parameters are:

- `IMT_QML_TOOLING_QT_ROOT`: host Qt installation; defaults to `QT_HOST_PATH`
  or the installation containing `Qt6::qmake` when available.
- `IMT_QML_TOOLING_PYTHON`: Python interpreter; defaults to `PYTHON_EXECUTABLE`
  or a discovered `python3`/`python`.
- `IMT_QML_TOOLING_OUTPUT`: editor import root; defaults to `<build-directory>/qml-tooling`.

Add the absolute output path to `qt-qml.qmlls.additionalImportPaths`, then run
**Qt: Restart QML Language Server**. Never add it to application import paths
or deployment.

## Garden Integration

Garden includes this ImtCore target separately from its product dependency
graph and sets the default output to `out/qml-tooling` under the GMG repository
root. Existing editor settings can therefore keep their import path.
From the GMG repository root, generate directly with:

```sh
python3 3rdParty/ImtCore/Config/QmlTooling/generate.py \
    --qt-root /path/to/Qt/6.x/host-kit --output out/qml-tooling
```

## Add Exports

[modules.json](../../Config/QmlTooling/modules.json) is the central tooling
registry. Its initial coverage is all ten exports registered by the two
`imtqml` registrator components under `com.imtcore.imtqml` 1.0. Optional runtime
registrations are described as available, irrespective of the application's
ACF configuration.

Add each additional native module's URI, version and types:

```json
{
    "uri": "example.models",
    "version": "1.0",
    "types": [
        {"name": "Model", "class": "example::CModel", "header": "Include/example/CModel.h"}
    ]
}
```

Header paths are relative to the **ImtCore repository root**. The header must
contain the named class's `Q_OBJECT` or `Q_GADGET`. Use `"singleton": true` for
singleton exports and `"uncreatable": true` for enum-only or other
non-creatable exports. Multiple classes in one header and aliases of a class
in different URIs are supported. Each URI has one version in this registry.

For modules also containing QML files, set `"qml_directory"` to their source
directory containing `qmldir`, relative to the ImtCore root. The generator
copies those files into the tooling output and preserves their QML exports.
Otherwise, the metadata-only module would shadow the original QML module.
List additional required Qt metatype libraries in `"qt_modules"`, for example
`["Quick3D"]`; Core, Gui, Qml, Quick, Network and WebSockets are included already.

Add future exports alongside their runtime registration calls and rerun the
target. Properties, signals and invokable methods are extracted by MOC from
the real headers. The generator adds export annotations to temporary MOC JSON,
runs Qt's internal `qmltyperegistrar`, and discards its registration C++ output.
No source header needs `QML_ELEMENT` or `QML_FOREIGN` added to it.

## Validation And Limits

From the ImtCore repository root:

```sh
python3 -m unittest discover -s Config/QmlTooling -p 'test_generate.py'
```

The tests compare the initial registry with the `imtqml` runtime calls so new
exports there cannot be silently omitted when these tests run.

Generation has been checked with Qt 6.8.3 and 6.11.1. Qt's internal JSON/tool
interfaces may change; revalidate after upgrades. Headers requiring generated
includes or build-specific macros need corresponding MOC preprocessing
support before being added. Generated SDL C++ registrations are not discovered
automatically.