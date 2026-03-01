# 3rdParty Migration - Quick Reference Card

## Prerequisites
- [ ] Git installed and configured
- [ ] GitHub CLI (`gh`) authenticated: `gh auth login`
- [ ] Admin access to ImagingTools organization
- [ ] Review ACTION_REQUIRED.md

## Step 1: Create Repositories (5 min)
```bash
./create-github-repos.sh create
```

## Step 2: Prepare Libraries (2 min)
```bash
./migrate-3rdparty.sh prepare-all
```

## Step 3: Push to GitHub (10 min)
```bash
cd /tmp/3rdparty-migration
for lib in EJ JQ JQML JQML2 QXlsx WebCompiler boost icu libmdbx mongoc mongocxx nodejs openssl pybind11 quazip zlib zstd; do
    cd "$lib"
    git remote add origin "https://github.com/ImagingTools/${lib}.git"
    git branch -M main
    git push -u origin main
    cd ..
done
```

## Step 4: Backup Original (1 min)
```bash
cd ~/work/ImtCore/ImtCore
tar -czf ~/3rdParty-backup-$(date +%Y%m%d).tar.gz 3rdParty/
```

## Step 5: Remove 3rdParty (1 min)
```bash
git rm -rf 3rdParty/
git commit -m "Remove 3rdParty directory for submodule migration"
```

## Step 6: Add Submodules (2 min)
```bash
./add-submodules.sh
git submodule update --init --recursive
```

## Step 7: Commit & Push (1 min)
```bash
git add .gitmodules
git commit -m "Convert 3rdParty libraries to git submodules"
git push
```

## Step 8: Verify (5 min)
```bash
git submodule status
ls -la 3rdParty/
# Test build
```

## Total Time: ~30 minutes

## Rollback if Needed
```bash
git rm -rf 3rdParty/ .gitmodules
tar -xzf ~/3rdParty-backup-*.tar.gz
git add 3rdParty/
git commit -m "Rollback: Restore 3rdParty directory"
```

## 17 Libraries to Migrate
EJ, JQ, JQML, JQML2, QXlsx, WebCompiler, boost, icu, libmdbx, mongoc, mongocxx, nodejs, openssl, pybind11, quazip, zlib, zstd

## After Migration - Developer Instructions
```bash
# New clones
git clone --recurse-submodules https://github.com/ImagingTools/ImtCore.git

# Existing clones
git pull
git submodule update --init --recursive
```

## Quick Help
- List libraries: `./migrate-3rdparty.sh list`
- Dry run: `./create-github-repos.sh dry-run`
- Single library: `./migrate-3rdparty.sh prepare <library>`
- Full docs: See STEP_BY_STEP_MIGRATION.md
