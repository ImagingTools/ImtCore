# Migration: 3rdParty Libraries to Git Submodules

## Overview

This document describes the process of migrating all libraries from the `3rdParty` directory to individual GitHub repositories and integrating them as git submodules.

## Libraries to Migrate

The following 14 libraries will be migrated to separate repositories:

1. **EJ** - Export library
2. **QXlsx** - Excel file library
3. **WebCompiler** - Python-based web compiler
4. **boost** (version 1.75.0) - C++ Boost libraries
5. **icu** (version 72.1) - International Components for Unicode
6. **libmdbx** - Memory-mapped database library
7. **mongoc** (version 1.23.2) - MongoDB C driver
8. **mongocxx** (version 3.7.0) - MongoDB C++ driver
9. **nodejs** - Node.js runtime and npm
10. **openssl** (version 1.1) - OpenSSL cryptography library
11. **pybind11** - Python/C++ binding library
12. **quazip** - Qt/C++ wrapper for ZIP/UNZIP
13. **zlib** (version 1.2.13) - Compression library
14. **zstd** (version 1.5.4) - Zstandard compression library

## Suggested Repository Naming Convention

Repositories should be named under the `ImagingTools` organization:

- `ImagingTools/EJ`
- `ImagingTools/QXlsx`
- `ImagingTools/WebCompiler`
- `ImagingTools/boost`
- `ImagingTools/icu`
- `ImagingTools/libmdbx`
- `ImagingTools/mongoc`
- `ImagingTools/mongocxx`
- `ImagingTools/nodejs`
- `ImagingTools/openssl`
- `ImagingTools/pybind11`
- `ImagingTools/quazip`
- `ImagingTools/zlib`
- `ImagingTools/zstd`

## Migration Steps

### Important: Git LFS Requirement

**All library repositories must be configured with Git LFS** (Large File Storage) to properly handle large binary files.

The following file types in the 3rdParty directory are tracked with Git LFS:
- `*.dll` - Windows dynamic libraries
- `*.lib` - Windows static libraries  
- `*.pyd` - Python extension modules
- `*.so` - Linux shared objects
- `*.dylib` - macOS dynamic libraries
- `*.exe` - Executable files
- `*.a` - Static archives

**Prerequisites:**
- Install Git LFS from https://git-lfs.github.com/
- Run `git lfs install` after installation
- The migration scripts automatically configure LFS for each repository

### Step 1: Create GitHub Repositories

For each library in the list above:

1. Create a new GitHub repository under `ImagingTools` organization
2. Initialize with a README describing the library and its version
3. Add an appropriate LICENSE file if the library has licensing requirements
4. Note the clone URL for later use

### Step 2: Populate Repositories

Use the provided migration script `migrate-3rdparty.sh` to:

1. Extract each library from the current `3rdParty` directory
2. Create a git repository for each library
3. Commit the library contents
4. Push to the newly created GitHub repository

**Manual approach (per library):**

```bash
# Example for EJ library
cd 3rdParty/EJ
git init

# Configure Git LFS
git lfs install
git lfs track "*.dll"
git lfs track "*.lib"
git lfs track "*.pyd"
git lfs track "*.so"
git lfs track "*.dylib"
git lfs track "*.exe"
git lfs track "*.a"

# Add and commit files
git add .gitattributes
git add .
git commit -m "Initial commit of EJ library with Git LFS support"
git remote add origin https://github.com/ImagingTools/EJ.git
git push -u origin main
```

### Step 3: Remove 3rdParty Directory

Before removing, ensure all repositories are created and populated:

```bash
# Backup first (optional but recommended)
tar -czf 3rdParty-backup-$(date +%Y%m%d).tar.gz 3rdParty/

# Remove the directory
git rm -rf 3rdParty/
```

### Step 4: Add Submodules

Use the provided `.gitmodules` configuration or add each submodule manually:

```bash
git submodule add https://github.com/ImagingTools/EJ.git 3rdParty/EJ
git submodule add https://github.com/ImagingTools/QXlsx.git 3rdParty/QXlsx
git submodule add https://github.com/ImagingTools/WebCompiler.git 3rdParty/WebCompiler
git submodule add https://github.com/ImagingTools/boost.git 3rdParty/boost
git submodule add https://github.com/ImagingTools/icu.git 3rdParty/icu
git submodule add https://github.com/ImagingTools/libmdbx.git 3rdParty/libmdbx
git submodule add https://github.com/ImagingTools/mongoc.git 3rdParty/mongoc
git submodule add https://github.com/ImagingTools/mongocxx.git 3rdParty/mongocxx
git submodule add https://github.com/ImagingTools/nodejs.git 3rdParty/nodejs
git submodule add https://github.com/ImagingTools/openssl.git 3rdParty/openssl
git submodule add https://github.com/ImagingTools/pybind11.git 3rdParty/pybind11
git submodule add https://github.com/ImagingTools/quazip.git 3rdParty/quazip
git submodule add https://github.com/ImagingTools/zlib.git 3rdParty/zlib
git submodule add https://github.com/ImagingTools/zstd.git 3rdParty/zstd
```

### Step 5: Initialize Submodules

After adding submodules, initialize and update them:

```bash
git submodule init
git submodule update
```

### Step 6: Commit Changes

```bash
git add .gitmodules
git commit -m "Convert 3rdParty libraries to git submodules"
git push
```

## Verification

After migration, verify:

1. All submodules are correctly checked out:
   ```bash
   git submodule status
   ```

2. Build system still works:
   ```bash
   # Run your build commands
   cmake ...
   make ...
   ```

3. All library references in CMake/QMake files still resolve correctly

## Build System References

The following files reference `3rdParty` paths and should continue to work with submodules:

- `Config/CMake/Mdbx.cmake`
- `Config/CMake/ImtCoreEnv.cmake`
- `Config/CMake/MongoCXX.cmake`
- `Config/CMake/EjExport.cmake`
- `Config/CMake/pybind11.cmake`
- `Config/CMake/WebCompiler.cmake`

No changes should be needed to these files as submodules will maintain the same directory structure.

## Benefits

1. **Independent Version Control**: Each library has its own git history
2. **Easier Updates**: Update individual libraries without affecting others
3. **Reduced Repository Size**: Main repository only tracks submodule references
4. **Better Organization**: Clear separation between main code and dependencies
5. **Flexible Dependencies**: Different projects can use different versions of the same library

## Considerations

1. **Initial Checkout**: Users must run `git submodule update --init --recursive` after cloning
2. **Updates**: Updating a submodule requires explicit commits in the parent repository
3. **CI/CD**: Build pipelines must be updated to initialize submodules
4. **Documentation**: Update README and developer documentation with submodule instructions

## Additional Resources

- [Git Submodules Documentation](https://git-scm.com/book/en/v2/Git-Tools-Submodules)
- [Working with Submodules](https://github.blog/2016-02-01-working-with-submodules/)

## Rollback Plan

If issues arise, the migration can be rolled back:

1. Remove all submodules: `git rm 3rdParty/*`
2. Remove .gitmodules: `git rm .gitmodules`
3. Restore from backup: `tar -xzf 3rdParty-backup-YYYYMMDD.tar.gz`
4. Add back to git: `git add 3rdParty/`
5. Commit: `git commit -m "Rollback: Restore 3rdParty directory"`
