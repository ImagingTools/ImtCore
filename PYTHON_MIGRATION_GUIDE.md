# Python Folder Migration Guide

This guide documents the migration of the `3rdParty/Python` folder to a separate submodule repository.

## Overview

The `3rdParty/Python` folder (approximately 1.5GB) has been moved to a separate repository to better manage this dependency as a Git submodule.

## Manual Steps Required

Since this migration requires creating a new GitHub repository and initializing it with the Python folder contents, please follow these steps:

### Step 1: Create the New Repository

1. Go to https://github.com/organizations/ImagingTools/repositories/new
2. Create a new repository named `Python`
3. Set it as **Private** (match the visibility of ImtCore)
4. Do NOT initialize with README, .gitignore, or license (we'll push existing content)

### Step 2: Initialize the New Repository with Python Folder

From the ImtCore repository directory, run:

```bash
# Navigate to the 3rdParty/Python folder
cd 3rdParty/Python

# Initialize a new git repository
git init

# Add all files
git add .

# Commit the files
git commit -m "Initial commit: Python 3.8 distribution from ImtCore"

# Add the remote (replace with actual repository URL)
git remote add origin https://github.com/ImagingTools/Python.git

# Push to the new repository
git branch -M main
git push -u origin main
```

### Step 3: Add Python as a Submodule in ImtCore

After the Python repository is created and populated:

```bash
# Navigate back to ImtCore root
cd /path/to/ImtCore

# Remove the Python folder (it's no longer tracked in this PR)
rm -rf 3rdParty/Python

# Add the Python repository as a submodule
git submodule add https://github.com/ImagingTools/Python.git 3rdParty/Python

# Commit the submodule addition
git add .gitmodules 3rdParty/Python
git commit -m "Add Python as submodule"

# Initialize and update the submodule
git submodule init
git submodule update
```

### Step 4: Update Documentation

Update the repository README.md to include submodule initialization instructions:

```markdown
## Cloning with Submodules

When cloning the repository for the first time:

```bash
git clone --recurse-submodules https://github.com/ImagingTools/ImtCore.git
```

Or if you already cloned without submodules:

```bash
git submodule init
git submodule update
```
```

## Files That Reference Python Path

The following files reference the `3rdParty/Python/3.8` path:

- `Config/CMake/ImtCoreEnv.cmake` - Sets `PYTHON38DIR` variable
- `Config/CMake/pybind11.cmake` - Includes and links Python libraries
- `Config/QMake/pybind11.pri` - QMake configuration for Python

**Important:** These files do NOT need to be modified because the path `3rdParty/Python/3.8` will remain the same after the submodule is added. The submodule will be checked out at `3rdParty/Python`, maintaining the same directory structure.

## Benefits of This Migration

1. **Smaller main repository**: Reduces the ImtCore repository size by ~1.5GB
2. **Separate version control**: Python distribution can be versioned independently
3. **Easier updates**: Python version updates can be managed in the separate repository
4. **Better dependency management**: Follows Git submodule best practices for large dependencies

## Verification

After completing the migration, verify that:

1. The Python submodule is properly initialized:
   ```bash
   git submodule status
   ```

2. The Python files are accessible:
   ```bash
   ls -la 3rdParty/Python/3.8/
   ```

3. CMake can find Python:
   ```bash
   # This should resolve correctly in your build
   cmake -DIMTCOREDIR=/path/to/ImtCore ...
   ```
