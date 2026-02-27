# 3rdParty Libraries Submodule Migration - Action Required

## Summary

This PR prepares the migration of all 17 third-party libraries from the monolithic `3rdParty` directory to individual git submodules. The migration has been **partially automated** with scripts and documentation, but **manual steps are required** to complete the process.

## What Has Been Done ✅

The following preparation work has been completed:

1. **Analysis**: Identified all 17 libraries in `3rdParty/` directory
2. **Documentation**: Created comprehensive migration guides
3. **Automation Scripts**: Created helper scripts for the migration
4. **Configuration**: Prepared `.gitmodules` template
5. **README Updates**: Updated main README with submodule instructions

### Files Added

- `MIGRATION_3RDPARTY_TO_SUBMODULES.md` - High-level migration overview and documentation
- `STEP_BY_STEP_MIGRATION.md` - Detailed step-by-step instructions
- `migrate-3rdparty.sh` - Script to prepare library repositories
- `create-github-repos.sh` - Script to create GitHub repositories (requires `gh` CLI)
- `.gitmodules.template` - Template for submodule configuration
- `README.md` - Updated with submodule usage instructions

## What Needs to Be Done Manually 🔧

The following steps require **GitHub organization permissions** and must be completed manually:

### Step 1: Create GitHub Repositories

You need to create 17 new repositories under the `ImagingTools` organization:

**Option A: Using GitHub CLI (Recommended)**

```bash
# Ensure gh is installed and authenticated
gh auth login

# Create all repositories automatically
./create-github-repos.sh create
```

**Option B: Manual Creation via GitHub Web Interface**

Create each repository manually at: https://github.com/organizations/ImagingTools/repositories/new

Repository names:
- `ImtCore-3rdParty-EJ`
- `ImtCore-3rdParty-JQ`
- `ImtCore-3rdParty-JQML`
- `ImtCore-3rdParty-JQML2`
- `ImtCore-3rdParty-QXlsx`
- `ImtCore-3rdParty-WebCompiler`
- `ImtCore-3rdParty-boost`
- `ImtCore-3rdParty-icu`
- `ImtCore-3rdParty-libmdbx`
- `ImtCore-3rdParty-mongoc`
- `ImtCore-3rdParty-mongocxx`
- `ImtCore-3rdParty-nodejs`
- `ImtCore-3rdParty-openssl`
- `ImtCore-3rdParty-pybind11`
- `ImtCore-3rdParty-quazip`
- `ImtCore-3rdParty-zlib`
- `ImtCore-3rdParty-zstd`

### Step 2: Populate Repositories

Prepare and push each library to its repository:

```bash
# Prepare all libraries (creates temp repos in /tmp/3rdparty-migration/)
./migrate-3rdparty.sh prepare-all

# Push each library to GitHub
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

### Step 3: Convert to Submodules

Once all repositories are created and populated:

```bash
cd /home/runner/work/ImtCore/ImtCore

# Backup first!
tar -czf ~/3rdParty-backup-$(date +%Y%m%d-%H%M%S).tar.gz 3rdParty/

# Remove 3rdParty directory
git rm -rf 3rdParty/
git commit -m "Remove 3rdParty directory in preparation for submodule migration"

# Add submodules (using generated script)
./add-submodules.sh

# Initialize and update
git submodule update --init --recursive

# Commit
git add .gitmodules
git commit -m "Convert 3rdParty libraries to git submodules"
git push
```

## Why Manual Steps Are Needed

This migration cannot be fully automated because:

1. **GitHub Repository Creation**: Creating repositories requires GitHub organization admin permissions and authentication that cannot be automated in this environment
2. **Git Pushing**: Pushing to new repositories requires GitHub credentials
3. **Repository Permissions**: Only organization admins can create repositories under `ImagingTools`

## Benefits of This Migration

Once complete, the migration will provide:

1. ✅ **Independent Version Control**: Each library has its own git history
2. ✅ **Easier Updates**: Update individual libraries without affecting others
3. ✅ **Reduced Repository Size**: Main repository only tracks submodule references
4. ✅ **Better Organization**: Clear separation between main code and dependencies
5. ✅ **Flexible Dependencies**: Different projects can use different versions

## Rollback Plan

If needed, the migration can be rolled back:

```bash
# Remove submodules
git rm -rf 3rdParty/
git rm .gitmodules

# Restore from backup
tar -xzf ~/3rdParty-backup-*.tar.gz

# Add back to git
git add 3rdParty/
git commit -m "Rollback: Restore 3rdParty directory"
git push
```

## Documentation Reference

- **[MIGRATION_3RDPARTY_TO_SUBMODULES.md](MIGRATION_3RDPARTY_TO_SUBMODULES.md)** - Complete migration overview
- **[STEP_BY_STEP_MIGRATION.md](STEP_BY_STEP_MIGRATION.md)** - Detailed step-by-step guide
- **[README.md](README.md)** - Updated with submodule usage instructions

## Next Steps

1. Review this PR and the documentation
2. If approved, merge this PR to get the migration scripts and documentation
3. Follow the manual steps in **STEP_BY_STEP_MIGRATION.md** to complete the migration
4. Update CI/CD pipelines to use `actions/checkout@v3` with `submodules: recursive`
5. Inform all developers about the change and submodule usage

## Questions?

If you have questions or need assistance, please:
1. Review the comprehensive documentation files
2. Check the troubleshooting section in STEP_BY_STEP_MIGRATION.md
3. Contact the repository maintainers

---

**Note**: This PR contains only documentation and scripts. The actual migration (repository creation and submodule conversion) must be performed separately by someone with appropriate permissions.
