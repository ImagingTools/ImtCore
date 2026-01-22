# Next Steps: Complete the Python Submodule Migration

This PR has prepared the ImtCore repository for moving the Python folder to a submodule. The following manual steps are required to complete the migration.

## What Has Been Done

✅ Removed `3rdParty/Python` from Git tracking (approximately 1.5GB, ~17,000 files)
✅ Physical Python folder preserved on disk at `3rdParty/Python`
✅ Updated `.gitignore` to temporarily ignore the Python folder
✅ Updated `README.md` with submodule initialization instructions
✅ Created comprehensive `PYTHON_MIGRATION_GUIDE.md`

## What You Need to Do

### 1. Create the New Repository

Create a new GitHub repository in the ImagingTools organization:
- **Name**: `Python`
- **Visibility**: Private (to match ImtCore)
- **DO NOT** initialize with README, .gitignore, or license

**Link**: https://github.com/organizations/ImagingTools/repositories/new

### 2. Push Python Folder to New Repository

After this PR is merged, run these commands from the ImtCore directory:

```bash
# Navigate to the Python folder
cd 3rdParty/Python

# Initialize a new git repository
git init

# Add all files
git add .

# Commit the files
git commit -m "Initial commit: Python 3.8 distribution from ImtCore"

# Add the remote (use the actual repository URL)
git remote add origin https://github.com/ImagingTools/Python.git

# Push to the new repository
git branch -M main
git push -u origin main

# Return to ImtCore root
cd ../..
```

### 3. Add Python as a Submodule

After the Python repository is populated:

```bash
# Remove the old Python folder
rm -rf 3rdParty/Python

# Add the Python repository as a submodule
git submodule add https://github.com/ImagingTools/Python.git 3rdParty/Python

# Commit the submodule addition
git add .gitmodules 3rdParty/Python
git commit -m "Add Python as submodule"

# Push the changes
git push
```

### 4. Verify the Setup

```bash
# Check submodule status
git submodule status

# Verify Python files are accessible
ls -la 3rdParty/Python/3.8/

# Test that build configuration can find Python
# (paths in CMake/QMake files remain unchanged)
```

## Important Notes

- **No code changes required**: All CMake and QMake files that reference `3rdParty/Python/3.8` will continue to work as-is because the path structure is preserved
- **Cloning for others**: After the submodule is added, users cloning the repository should use:
  ```bash
  git clone --recurse-submodules https://github.com/ImagingTools/ImtCore.git
  ```
  Or if already cloned:
  ```bash
  git submodule init
  git submodule update
  ```

## Files That Reference Python Path

These files reference the Python path but do NOT need modification:
- `Config/CMake/ImtCoreEnv.cmake` - Sets `PYTHON38DIR` variable
- `Config/CMake/pybind11.cmake` - Includes and links Python libraries
- `Config/QMake/pybind11.pri` - QMake configuration for Python

The path `3rdParty/Python/3.8` will be identical after the submodule is checked out.

## Benefits of This Change

1. **Reduced repository size**: Main repository is ~1.5GB smaller
2. **Independent versioning**: Python distribution can be versioned separately
3. **Easier updates**: Python updates can be managed in the separate repository
4. **Better organization**: Follows Git best practices for large dependencies

## Questions or Issues?

Refer to `PYTHON_MIGRATION_GUIDE.md` for more detailed information about the migration process.
