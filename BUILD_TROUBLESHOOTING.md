# Build Troubleshooting Guide

## Critical: New Library Added in PR #130

**Important:** The imtsdlgencpp library and related files were completely new additions in PR #130. If you're experiencing build errors, you MUST reconfigure your build system from scratch.

## Quick Fix

**For CMake builds:**
```cmd
# Windows - Delete build directory
rd /s /q Build\VC17_64

# Create new build directory and configure
mkdir Build\VC17_64
cd Build\VC17_64
cmake ..\CMake

# Build
cmake --build .
```

**For QMake builds:**
```cmd
# Clean all generated files
qmake -r Build\QMake\ImtCoreAll.pro
nmake distclean

# Reconfigure and build
qmake -r Build\QMake\ImtCoreAll.pro  
nmake
```

## Common Build Issues and Solutions

### Issue: Unresolved External Symbols for CBaseClassExtenderComp

**Symptoms:**
```
error LNK2001: Nicht aufgelöstes externes Symbol "CBaseClassExtenderComp::DoProcessing"
error LNK2001: Nicht aufgelöstes externes Symbol "CBaseClassExtenderComp::ProcessHeaderClassFile"
... (and other CBaseClassExtenderComp methods)
```

**Cause:**
The ACF StaticConfig.cmake uses file globbing to discover source files at CMake configuration time. When new `.cpp` files are added to the repository (like `CBaseClassExtenderComp.cpp`), CMake may not automatically pick them up if the configuration cache is stale.

**Solution:**
1. Delete the CMake cache and build directory:
   ```cmd
   # Windows
   rd /s /q Build\VC17_64
   ```
   ```bash
   # Linux/macOS
   rm -rf Build/ClangOSX_*
   ```

2. Reconfigure CMake from scratch:
   ```cmd
   # Windows - from Build\VC17_64 directory
   cmake ..\CMake
   ```
   ```bash
   # Linux/macOS - from build directory
   cmake ../CMake
   ```

3. Rebuild the project

### Issue: Missing Generated Header Files

**Symptoms:**
```
error C1083: Datei (Include) kann nicht geöffnet werden: "GeneratedFiles/ImtSdlGenTest/CImtSdlGenTest.h"
```

**Cause:**
The `.acc` (ArchiCompany Component) files need to be processed by the ACF compiler before the test executables are compiled. This is a build dependency issue.

**Solution:**
1. Ensure the `.acc` files are being processed:
   - Check that `CustomBuild.cmake` is included in the test CMakeLists.txt
   - Verify that ARXC_CONFIG, ARXC_FILES, and ARXC_OUTDIR are correctly set

2. Build the dependent libraries first:
   ```cmd
   # Build libraries before tests
   cmake --build . --target imtsdlgencpp
   cmake --build . --target imtsdlgenqml
   # Then build tests
   cmake --build . --target ImtSdlGenTest
   ```

3. If the issue persists, clean and rebuild:
   ```cmd
   cmake --build . --target clean
   cmake --build .
   ```

### Issue: Missing mdbx.h++ Header

**Symptoms:**
```
error C1083: Datei (Include) kann nicht geöffnet werden: "mdbx.h++"
```

**Cause:**
The include path for the libmdbx 3rdParty library is not being set correctly. This should be handled by `Mdbx.cmake`.

**Solution:**
1. Verify that the mdbx header exists:
   ```cmd
   dir 3rdParty\libmdbx\include\mdbx.h++
   ```

2. Check that Mdbx.cmake is included in the test CMakeLists.txt

3. Reconfigure CMake to ensure include paths are updated

## General Build Requirements

### External Dependencies

1. **ACF (ArchiCompany Framework)**
   - Required environment variables: `ACFDIR`, `IACFDIR`
   - Must be installed before building ImtCore

2. **Qt Framework**
   - Qt 5.15 or Qt 6.x required
   - Qt should be discoverable by CMake's find_package()

3. **Python 3.8+**
   - Set `PYTHON38DIR` environment variable
   - Used for build scripts and code generation

### CMake Configuration Order

The correct CMake configuration order is:
1. External framework configuration (ACF)
2. Static library configuration (StaticConfig.cmake)
3. Project-specific configuration (ImtCore.cmake)
4. 3rdParty library configuration (Mdbx.cmake, etc.)

If you modify any CMakeLists.txt files or add new source files, always reconfigure CMake to ensure the changes are picked up.

## Getting Help

If you continue to experience build issues:
1. Check that all environment variables are set correctly
2. Ensure all git submodules are initialized and updated
3. Run `git lfs pull` to ensure all large files are downloaded
4. Try a completely clean build from scratch
5. Check the CI build logs for reference configuration

## See Also

- [README.md](README.md) - Environment setup and initial configuration
- [Build/Git/README.md](Build/Git/README.md) - Git hooks and version management
