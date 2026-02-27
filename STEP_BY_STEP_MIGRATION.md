# Step-by-Step Migration Guide

This guide provides detailed instructions for migrating the 3rdParty libraries to git submodules.

## Prerequisites

- Git installed and configured
- Access to create repositories in the `ImagingTools` GitHub organization
- GitHub credentials configured (SSH or HTTPS)

## Phase 1: Prepare Library Repositories

### Step 1.1: Run the Migration Script

The `migrate-3rdparty.sh` script will prepare temporary git repositories for each library:

```bash
./migrate-3rdparty.sh prepare-all
```

This will:
- Create a temporary directory `/tmp/3rdparty-migration/`
- Copy each library from `3rdParty/` to its own directory
- Initialize a git repository for each
- Create a basic README.md
- Make an initial commit

### Step 1.2: Verify Prepared Repositories

Check the prepared repositories:

```bash
ls -la /tmp/3rdparty-migration/
```

You should see 17 directories, one for each library.

## Phase 2: Create GitHub Repositories

You need to create 17 repositories under the `ImagingTools` organization. You can do this via:

### Option A: GitHub Web Interface

For each library:

1. Go to https://github.com/organizations/ImagingTools/repositories/new
2. Repository name: `ImtCore-3rdParty-{LIBRARY_NAME}` (e.g., `ImtCore-3rdParty-EJ`)
3. Description: "3rdParty dependency for ImtCore: {LIBRARY_NAME}"
4. Visibility: Public or Private (based on your organization policy)
5. Do NOT initialize with README, .gitignore, or license
6. Click "Create repository"

### Option B: GitHub CLI (gh)

If you have GitHub CLI installed:

```bash
# Login if needed
gh auth login

# Create repositories
gh repo create ImagingTools/ImtCore-3rdParty-EJ --public
gh repo create ImagingTools/ImtCore-3rdParty-JQ --public
gh repo create ImagingTools/ImtCore-3rdParty-JQML --public
gh repo create ImagingTools/ImtCore-3rdParty-JQML2 --public
gh repo create ImagingTools/ImtCore-3rdParty-QXlsx --public
gh repo create ImagingTools/ImtCore-3rdParty-WebCompiler --public
gh repo create ImagingTools/ImtCore-3rdParty-boost --public
gh repo create ImagingTools/ImtCore-3rdParty-icu --public
gh repo create ImagingTools/ImtCore-3rdParty-libmdbx --public
gh repo create ImagingTools/ImtCore-3rdParty-mongoc --public
gh repo create ImagingTools/ImtCore-3rdParty-mongocxx --public
gh repo create ImagingTools/ImtCore-3rdParty-nodejs --public
gh repo create ImagingTools/ImtCore-3rdParty-openssl --public
gh repo create ImagingTools/ImtCore-3rdParty-pybind11 --public
gh repo create ImagingTools/ImtCore-3rdParty-quazip --public
gh repo create ImagingTools/ImtCore-3rdParty-zlib --public
gh repo create ImagingTools/ImtCore-3rdParty-zstd --public
```

## Phase 3: Push Library Contents to GitHub

For each library, push the prepared repository to GitHub:

```bash
cd /tmp/3rdparty-migration/EJ
git remote add origin https://github.com/ImagingTools/ImtCore-3rdParty-EJ.git
git branch -M main
git push -u origin main
cd ..

cd /tmp/3rdparty-migration/JQ
git remote add origin https://github.com/ImagingTools/ImtCore-3rdParty-JQ.git
git branch -M main
git push -u origin main
cd ..

# Repeat for all libraries...
```

Or use this loop:

```bash
cd /tmp/3rdparty-migration

for lib in EJ JQ JQML JQML2 QXlsx WebCompiler boost icu libmdbx mongoc mongocxx nodejs openssl pybind11 quazip zlib zstd; do
    echo "Pushing $lib..."
    cd "$lib"
    git remote add origin "https://github.com/ImagingTools/ImtCore-3rdParty-${lib}.git"
    git branch -M main
    git push -u origin main
    cd ..
done
```

## Phase 4: Remove 3rdParty Directory from ImtCore

**IMPORTANT: Make a backup first!**

```bash
cd /home/runner/work/ImtCore/ImtCore

# Create backup
tar -czf ~/3rdParty-backup-$(date +%Y%m%d-%H%M%S).tar.gz 3rdParty/

# Remove from git
git rm -rf 3rdParty/

# Commit the removal
git commit -m "Remove 3rdParty directory in preparation for submodule migration"
```

## Phase 5: Add Submodules

Now add all libraries as git submodules:

```bash
cd /home/runner/work/ImtCore/ImtCore

# Add each library as a submodule
git submodule add https://github.com/ImagingTools/ImtCore-3rdParty-EJ.git 3rdParty/EJ
git submodule add https://github.com/ImagingTools/ImtCore-3rdParty-JQ.git 3rdParty/JQ
git submodule add https://github.com/ImagingTools/ImtCore-3rdParty-JQML.git 3rdParty/JQML
git submodule add https://github.com/ImagingTools/ImtCore-3rdParty-JQML2.git 3rdParty/JQML2
git submodule add https://github.com/ImagingTools/ImtCore-3rdParty-QXlsx.git 3rdParty/QXlsx
git submodule add https://github.com/ImagingTools/ImtCore-3rdParty-WebCompiler.git 3rdParty/WebCompiler
git submodule add https://github.com/ImagingTools/ImtCore-3rdParty-boost.git 3rdParty/boost
git submodule add https://github.com/ImagingTools/ImtCore-3rdParty-icu.git 3rdParty/icu
git submodule add https://github.com/ImagingTools/ImtCore-3rdParty-libmdbx.git 3rdParty/libmdbx
git submodule add https://github.com/ImagingTools/ImtCore-3rdParty-mongoc.git 3rdParty/mongoc
git submodule add https://github.com/ImagingTools/ImtCore-3rdParty-mongocxx.git 3rdParty/mongocxx
git submodule add https://github.com/ImagingTools/ImtCore-3rdParty-nodejs.git 3rdParty/nodejs
git submodule add https://github.com/ImagingTools/ImtCore-3rdParty-openssl.git 3rdParty/openssl
git submodule add https://github.com/ImagingTools/ImtCore-3rdParty-pybind11.git 3rdParty/pybind11
git submodule add https://github.com/ImagingTools/ImtCore-3rdParty-quazip.git 3rdParty/quazip
git submodule add https://github.com/ImagingTools/ImtCore-3rdParty-zlib.git 3rdParty/zlib
git submodule add https://github.com/ImagingTools/ImtCore-3rdParty-zstd.git 3rdParty/zstd
```

Or use the generated script:

```bash
./add-submodules.sh
```

## Phase 6: Initialize and Verify Submodules

```bash
# Initialize all submodules
git submodule init

# Update all submodules
git submodule update

# Check submodule status
git submodule status

# Verify directory structure
ls -la 3rdParty/
```

## Phase 7: Commit and Push

```bash
# Add .gitmodules file
git add .gitmodules

# Commit the submodule configuration
git commit -m "Convert 3rdParty libraries to git submodules

All 17 third-party libraries have been moved to separate repositories
and integrated as git submodules:

- EJ, JQ, JQML, JQML2, QXlsx
- WebCompiler
- boost, icu, libmdbx
- mongoc, mongocxx
- nodejs, openssl
- pybind11, quazip
- zlib, zstd

This allows independent version control and easier dependency management."

# Push to GitHub
git push origin <branch-name>
```

## Phase 8: Verify Build

Test that the build still works with submodules:

```bash
# Clean any previous build artifacts
rm -rf build/

# Configure build
cmake -B build -S .

# Build
cmake --build build

# Run tests if applicable
ctest --test-dir build
```

## Phase 9: Update CI/CD and Documentation

### Update CI/CD Pipelines

Ensure CI/CD workflows initialize submodules:

```yaml
# GitHub Actions example
- name: Checkout code with submodules
  uses: actions/checkout@v3
  with:
    submodules: recursive
```

### Update README

Add submodule instructions to README:

```markdown
## Cloning

When cloning this repository, include submodules:

\`\`\`bash
git clone --recurse-submodules https://github.com/ImagingTools/ImtCore.git
\`\`\`

Or if already cloned:

\`\`\`bash
git submodule update --init --recursive
\`\`\`
```

## Troubleshooting

### Submodule not initialized

```bash
git submodule update --init --recursive
```

### Submodule shows modified

```bash
cd 3rdParty/<library>
git status
# If changes exist, either commit them or reset
git reset --hard HEAD
```

### Update a submodule to latest version

```bash
cd 3rdParty/<library>
git pull origin main
cd ../..
git add 3rdParty/<library>
git commit -m "Update <library> submodule"
git push
```

### Detached HEAD in submodule

This is normal for submodules. They point to specific commits, not branches.
To work on a submodule:

```bash
cd 3rdParty/<library>
git checkout main
# Make changes, commit, push
cd ../..
git add 3rdParty/<library>
git commit -m "Update <library> submodule"
```

## Cleanup

After successful migration:

```bash
# Remove temporary migration directory
rm -rf /tmp/3rdparty-migration

# Remove backup (only after verifying everything works!)
# rm ~/3rdParty-backup-*.tar.gz
```

## Rollback

If you need to rollback:

```bash
# Remove submodules
git rm -rf 3rdParty/
git rm .gitmodules

# Restore from backup
cd ~
tar -xzf 3rdParty-backup-*.tar.gz
mv 3rdParty /home/runner/work/ImtCore/ImtCore/

# Add back to git
cd /home/runner/work/ImtCore/ImtCore
git add 3rdParty/
git commit -m "Rollback: Restore 3rdParty directory"
git push origin <branch-name>
```
