# SDL C++ generation: the `<Schema>Objects` files

Русская версия: [SDL_Objects_Generation_ru.md](SDL_Objects_Generation_ru.md)

This document describes how the SDL generator splits its C++ output, and how to disable the QObject based part (`NO_OBJECTS`).

## 1. Generated files

For a schema `Foo` the generator creates:

| File | Content |
|------|---------|
| `Foo.h`, `Foo.cpp` | Plain data classes (`CType`), enums (`Q_NAMESPACE`, `Q_ENUM_NS`, `EnumXxx` wrappers), serialization, requests |
| `Foo_fwd.h` | Forward declarations |
| `FooObjects.h` | QObject based classes: `CTypeObject`, `CTypeObjectList`, union objects, `Register<Schema>QmlTypes()` |
| `moc_Foo.cpp`, `moc_FooObjects.cpp` | moc output, created by the build system (CMake) |

`FooObjects.cpp` is an **intermediate** file: its content is appended to `Foo.cpp` by the post processor, and the file is removed afterwards (on success and on failure).
It is not an output of the build.

With `--include-moc` (always passed by `ImtCoreCustomConfigureSdlCpp`) the end of `Foo.cpp` includes the moc output:

```cpp
#include "moc_Foo.cpp"         // always: the meta objects of the main header
#include "moc_FooObjects.cpp"  // only if the objects are generated
```

So there is one translation unit per schema, and the moc files are not compiled separately.

## 2. Disabling the objects

The QObject classes are needed only to use the SDL types from QML **through C++ models** (the classes are registered in QML by the application code, see `Register<Schema>QmlTypes()`). They are not used by the QML part of the generator: it creates its own QML types (the `.qml` files and `qmldir`) independently of the C++ classes. So a schema, that is used from C++ without these models, or from QML only, may be generated without them.

### CMake

```cmake
ImtCoreCustomConfigureSdlCpp(
	NO_OBJECTS
	SCHEMA_PATH "${CMAKE_CURRENT_LIST_DIR}/../1.0/Foo.sdl"
	VERSION "1.0")

# the same flag of the 'CppQml' macro (C++ only)
ImtCoreCustomConfigureSdlCppQml(CXX NO_OBJECTS
	SCHEMA_PATH "${CMAKE_CURRENT_LIST_DIR}/../1.0/Foo.sdl"
	VERSION "1.0")
```

* `NO_OBJECTS` is acceptable together with `QML`: it affects only the C++ generator, the QML one is not changed. This is the case for a QML only application.
* The generator gets `--no-objects`; `FooObjects.h` is not created, not declared as an output and its moc is not generated.

### Command line

| Option | Meaning |
|--------|---------|
| `--no-objects` | Do not generate `<Schema>Objects` files |
| `--include-moc` | Include the moc output at the end of the generated source file |

## 3. Rules and limitations

1. **Imports.** The objects of a schema include the objects of the imported schemas (`BarObjects.h`). So a schema **with** objects can not import a schema generated **without** them. A schema without objects may import any schema.

   The generator can not know, how the imported schema was generated (it may be a schema of another project, built separately). So the check is done by the compiler: every objects header of an imported schema is included in the guarded form

   ```cpp
   #if __has_include(<barsdl/SDL/1.0/CPP/BarObjects.h>)
   #include <barsdl/SDL/1.0/CPP/BarObjects.h>
   #else
   #error The objects header <barsdl/SDL/1.0/CPP/BarObjects.h> is not found. The schema, that defines the imported types, is probably generated without objects (NO_OBJECTS). ...
   #endif
   ```

   so instead of "file not found" the compilation fails with the reason. The guard is not applied to the own libraries (`imtbase`, `imtsdl`).
2. **One flag for the schemas of a project that import each other.** For example, if `Rs485.sdl` imports `FrameStructure.sdl`, either both or none have `NO_OBJECTS`.
3. **Output collisions.** `FooObjects.h` is the objects header of `Foo` and the main header of a schema named `FooObjects`. CMake reports `SDL output file collision` if two schemas produce the same file. Do not name a schema `<Other>Objects`.
4. **The suffix `Objects`** is defined in two places, keep them equal: `ISdlProcessArgumentsParser::s_objectsFileSuffix` (C++) and `IMT_SDL_OBJECTS_SUFFIX` (`ImtCoreSdlCustomConfig.cmake`).
5. **QMake** SDL integration (`Config/QMake/SdlConfiguration.pri`) is not supported for the objects (`<Schema>Objects.h` is not moc-ed, `--cpp-mode=IMPL` is not supported).

## 4. Migration notes

* `C<Type>Object`, `C<Type>ObjectList`, union objects and `Register<Schema>QmlTypes()` moved from `Foo.h` to `FooObjects.h`. Code that uses them must include `FooObjects.h`.
* `Foo.h` does not include `<imtsdl/TListModelBase.h>` and `<imtbase/CItemModelBase.h>` anymore. Include them directly if they are used.
* A project that compiles the generated sources by itself (not through `ImtCoreCustomConfigureSdlCpp`) must pass `--include-moc` and create the moc files next to the generated source, otherwise the QObject classes are not linked.

## 5. Tests

| Test | What it checks | Run |
|------|----------------|-----|
| `ImtSdlGenTest` | The generator: files, moc includes, depfile, target paths, the guarded includes of the imported objects (also checked by the compiler), removing of the intermediate file | `ImtSdlGenTest` (needs `IMTCOREDIR`) |
| `ImtSdlGenIntegrationTest` | The generated code with the real libraries, including `NO_OBJECTS` (`NoObjects.sdl`, `NoObjectsConsumer.sdl`) | CMake option `IMT_BUILD_SDL_INTEGRATION_TESTS=ON` (requires `BUILD_TESTING`, `USE_QTDB`, `IMT_BUILD_TOOLS_ONLY=OFF`) |
| `ImtSdlCMakeMacros` | The CMake macros: forwarding of `NO_OBJECTS` (the QML generator is not affected), independence of the flags of the schemas, output collisions | `ctest --test-dir <build dir of ImtSdlGenIntegrationTest>` (needs `ninja`) |
