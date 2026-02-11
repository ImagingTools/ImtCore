# ImtCore

## Documentation

**API Documentation**: [https://imagingtools.github.io/ImtCore/](https://imagingtools.github.io/ImtCore/)  
Comprehensive technical documentation generated with Doxygen, including class hierarchies, API references, and detailed descriptions.

**Repository Statistics**: [https://imagingtools.github.io/ImtCore/stats/](https://imagingtools.github.io/ImtCore/stats/)  
View comprehensive code metrics including lines of code, number of classes and components, code quality indicators, and more.

The documentation and statistics are automatically generated from the source code and deployed to GitHub Pages.

## License Management

ImtCore includes a comprehensive three-module licensing system for managing software and hardware product licenses:

### Modules

- **imtlic** - Core licensing domain model implementing feature-based licensing
- **imtlicgql** - GraphQL API layer for web-based license management
- **imtlicdb** - PostgreSQL persistence layer with JSON document storage

### Key Features

- **Feature-Based Licensing**: Atomic units of functionality unlocked through licenses
- **Product-Instance Separation**: Clear distinction between product definitions and customer deployments
- **Temporal Control**: License instances with expiration dates and goodwill periods
- **Hardware Binding**: Support for hardware-locked licenses
- **Hierarchical Organization**: Features can form trees, products can be in families
- **Dependency Management**: Features and licenses can depend on each other
- **GraphQL API**: Modern web API with JavaScript/TypeScript/Python client support

### Architecture

The licensing system follows a clean three-layer architecture:

```
GraphQL API (imtlicgql)
    ↓
Domain Model (imtlic)
    ↓
Database Layer (imtlicdb)
    ↓
PostgreSQL
```

### Core Concepts

**Definition-Instance Pattern:**
- **Definitions** (what can be sold): Products, Features, License Definitions
- **Instances** (what was sold): Product Instances, License Instances with expiration

**Container Pattern:**
- Products contain Features (all capabilities)
- License Definitions contain Features (subset that gets unlocked)
- Product Instances contain License Instances (activated licenses)

### Documentation

- **[LICENSING_ARCHITECTURE.md](LICENSING_ARCHITECTURE.md)** - Complete system architecture with design patterns and data flows
- **[Include/imtlic/README.md](Include/imtlic/README.md)** - Core domain model with C++ usage examples
- **[Include/imtlicgql/README.md](Include/imtlicgql/README.md)** - GraphQL API with query/mutation examples
- **[Include/imtlicdb/README.md](Include/imtlicdb/README.md)** - Database schema and SQL examples
- **[LICENSING_DOCUMENTATION_SUMMARY.md](LICENSING_DOCUMENTATION_SUMMARY.md)** - Documentation overview and metrics

### Usage Example

```cpp
// Create a product with features
IProductInfoSharedPtr product = ...; // From factory
product->SetProductId("product-office-suite");
product->SetName("Office Suite");

// Add features
IFeatureInfoSharedPtr feature = ...;
feature->SetFeatureId("feature-advanced-analytics");
feature->SetFeatureName("Advanced Analytics");
feature->SetIsOptional(true);  // Can be sold separately
product->AddFeature(feature->GetFeatureId(), *feature);

// Create license definition
ILicenseDefinitionSharedPtr license = ...;
license->SetLicenseId("license-professional");
license->SetLicenseName("Professional License");
license->AddFeature("feature-advanced-analytics", "Advanced Analytics");
```

For detailed examples and API documentation, see the module-specific README files linked above.

## Environment Variables Setup

Before building the project, you need to set the following environment variables:

### PYTHON38DIR
Set this to the path of your Python 3.8 installation directory (the directory containing `include/` and `libs/` subdirectories).

#### Windows (PowerShell)
```powershell
$env:PYTHON38DIR = "C:\Python38"
# Or set permanently:
[System.Environment]::SetEnvironmentVariable('PYTHON38DIR', 'C:\Python38', 'User')
```

#### Windows (Command Prompt)
```cmd
set PYTHON38DIR=C:\Python38
# Or set permanently:
setx PYTHON38DIR "C:\Python38"
```

#### Linux/macOS
```bash
export PYTHON38DIR="/usr/local/lib/python3.8"
# Or add to ~/.bashrc or ~/.zshrc:
echo 'export PYTHON38DIR="/usr/local/lib/python3.8"' >> ~/.bashrc
```

### PYTHONEXE (Optional)
Set this to specify the Python executable path. If not set, the build system will default to `python.exe` (Windows) or `python3` (Linux/macOS).

#### Windows
```cmd
set PYTHONEXE=C:\Python38\python.exe
```

#### Linux/macOS
```bash
export PYTHONEXE=/usr/bin/python3
```

## Initial Setup

After cloning the repository for the first time, follow these steps:

### 1. Install Git LFS

This repository uses Git LFS (Large File Storage) for binary files in the `3rdParty` directory. If you encounter issues with LFS files, ensure Git LFS is installed:

#### Windows

```cmd
# Download and install Git LFS from https://git-lfs.github.com/
# Or use Chocolatey:
choco install git-lfs

# After installation, run:
git lfs install
```

#### Linux

```bash
# For Ubuntu/Debian:
sudo apt-get install git-lfs

# For Fedora/RHEL:
sudo dnf install git-lfs

# For Arch Linux:
sudo pacman -S git-lfs

# After installation, run:
git lfs install
```

If you cloned the repository before installing Git LFS, pull the LFS files:

```bash
git lfs pull
```

### 2. Install Git Hooks

To automatically update version files after each `git pull`, install the post-merge hook:

#### Windows

```cmd
cd Build\Git
InstallHooks.bat
```

#### Linux/macOS

```bash
cd Build/Git
bash InstallHooks.sh
```

This will install a hook that automatically runs the `UpdateVersion` script after each merge/pull operation.

### 3. Update Version Files (Manual)

If you need to manually update version files (for example, after switching branches or making local changes):

#### Windows

```cmd
cd Build\Git
UpdateVersion.bat
```

#### Linux/macOS

```bash
cd Build/Git
bash UpdateVersion.sh
```

## Version Management

The version information is stored in `Partitura/ImtCoreVoce.arp/VersionInfo.acc` and is automatically generated from the template file `VersionInfo.acc.xtrsvn` based on the git repository state:

- **Revision number**: Based on the commit count from `origin/master` (+ 10000 offset)

The UpdateVersion script replaces the `$WCREV$` placeholder in the template with the actual revision number.

The `post-merge` hook (installed by `InstallHooks`) ensures that version files are updated automatically after each `git pull`.

For more details about the Git build scripts, see [Build/Git/README.md](Build/Git/README.md).
