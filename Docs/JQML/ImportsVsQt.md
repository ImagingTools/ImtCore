# Imports in JQML v4 compared with Qt

The tests `Import_002` ... `Import_024` and `Singleton_005`, `Singleton_006` in `Tools/JQML/v4/test/jqml` run the same
`Main.qml` with desktop Qt (6.11.2, `qml -I <dir> ...`) and with the engine and compare what it logs:

```
node test/desktop/run.mjs Import_007_two_modules_same_type
```

The folders of `test.json` are import paths, a folder that contains `<Module>/qmldir`. The runner gives them to Qt as
`-I` (the last `-I` is searched first) and to the engine as `importPaths` in the order Qt searches them. All of these
tests give the same result in Qt and in the engine.

## The rules (the same in Qt and in the engine)

| Rule | Test |
| --- | --- |
| a module of an import path: its types and singletons | `Import_002` |
| the same module in two import paths: one of them is the module (the last `-I`), they are not merged | `Import_005`, `Import_006` |
| `import Mod 1.0 as M`: `M.Type` and `M.Singleton`; two imports with the same alias share it, the later import wins | `Import_003`, `Import_022`, `Import_023` |
| `import "dir"` and `import "dir" as D`: the .qml files of the directory (also those that its qmldir does not list) and the singletons of its qmldir (global names, or `D.Name`) | `Import_004`, `Import_014`, `Import_017` |
| the same type or singleton in two imports: the **later import** wins | `Import_007`, `Import_008`, `Import_016` |
| every import wins over a type of the folder of the file (a module and a directory) | `Import_009`, `Import_015` |
| the versions of a type or of a singleton in a qmldir: the newest one of the imported major version that is not above the imported version, `import Mod` takes the newest | `Import_010` ... `Import_012`, `Import_018` ... `Import_021` |
| a file sees its own imports, not those of the file that uses it | `Import_024` |
| `import "file.js" as Name` | `Import_013` |
| the `dirs` of a config of the compiler and of its `includes` are import paths | `Singleton_005`, `Singleton_006` |

A name that only one file declares as a singleton is a global name, like before. Only the names that several files
declare (different modules or directories, or several versions) are resolved per file, and the members of such a
singleton are not folded into constants.

The engine is more tolerant than Qt in one thing: an import of a version that no declaration fits gets the newest one.

## The config of the compiler

The compiler does not use import paths. `dirs` of the config (and of its `includes`, visited first) are the folders of
the qmldir files; the folders that declare the same module are merged into it. A type or a singleton that several
of them declare in the same version comes from the **last** folder, so an application overrides a library, like in the
JQML v3 compiler (`test/modules.mjs`). Qt has no such merge: it uses one folder per module.
