# Git Build Scripts

This directory contains scripts for managing version information and git hooks.

## Scripts

### UpdateVersion (Windows: .bat, Linux/macOS: .sh)

Updates the version information in `Partitura/ImtCoreVoce.arp/VersionInfo.acc` based on the git repository state.

- Replaces `$WCREV$` with the git revision count
- Replaces `$WCMODS?1:0$` with the dirty flag (1 if there are uncommitted changes, 0 otherwise)

**Usage:**
- Windows: `UpdateVersion.bat`
- Linux/macOS: `bash UpdateVersion.sh`

### InstallHooks (Windows: .bat, Linux/macOS: .sh)

Installs the `post-merge` hook into `.git/hooks/` directory.

**Usage:**
- Windows: `InstallHooks.bat`
- Linux/macOS: `bash InstallHooks.sh`

### post-merge

A git hook that automatically runs after a merge operation. It detects the operating system and calls the appropriate UpdateVersion script.

This hook is automatically installed by the `InstallHooks` script.

## Platform Support

- **Windows**: Uses `.bat` scripts
- **Linux/macOS**: Uses `.sh` scripts
- The `post-merge` hook automatically detects the platform and runs the correct script
