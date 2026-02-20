# ImtCore

## Documentation

**API Documentation**: [https://imagingtools.github.io/ImtCore/](https://imagingtools.github.io/ImtCore/)  
Comprehensive technical documentation generated with Doxygen, including class hierarchies, API references, and detailed descriptions.

**Repository Statistics**: [https://imagingtools.github.io/ImtCore/stats/](https://imagingtools.github.io/ImtCore/stats/)  
View comprehensive code metrics including lines of code, number of classes and components, code quality indicators, and more.

The documentation and statistics are automatically generated from the source code and deployed to GitHub Pages.

## Technical Domains Overview

ImtCore SDK consists of 61 libraries organized into 14 technical domains. Each domain provides specialized functionality for different aspects of software development.

### 1. Core Infrastructure
Foundation libraries providing base functionality used throughout the system.
- **7 libraries**: imtbase, imtcore, imtcom, imtcol, imttest, imtfile, imtzip
- **Documentation**: [Include/README.md - Core Infrastructure](Include/README.md#core-infrastructure-7-libraries)

### 2. Authentication & Authorization
Complete authentication and authorization system with multiple persistence backends.
- **4 libraries**: imtauth, imtauthdb, imtauthgql, imtauthgui
- **Documentation**: [Include/README.md - Authentication](Include/README.md#authentication--authorization-4-libraries)

### 3. Database & Persistence
SQL and NoSQL database connectivity and management.
- **5 libraries**: imtdb, imtdbgql, imtdbgui, imtmdbx, imtmongo
- **Documentation**: [Include/README.md - Database](Include/README.md#database--persistence-5-libraries)

### 4. Licensing System
Software and hardware product license management with GraphQL API and database persistence.
- **4 libraries**: imtlic, imtlicdb, imtlicgql, imtlicgui
- **Documentation**: [License Management](#license-management) (below), [Include/imtlic/README.md](Include/imtlic/README.md), [LICENSING_ARCHITECTURE.md](LICENSING_ARCHITECTURE.md)

### 5. GraphQL APIs
GraphQL-based API layer for client-server communication.
- **8 libraries**: imtgql, imtclientgql, imtservergql, imtguigql, imtgqltest, plus domain-specific APIs
- **Documentation**: [Include/README.md - GraphQL APIs](Include/README.md#graphql-apis-8-libraries), [Include/imtservergql/README.md](Include/imtservergql/README.md)

### 6. User Interface
Qt/QML-based user interface components and frameworks.
- **16 libraries**: Core UI (imtgui, imtwidgets, imtstyle, imtqml, imtqml2d, imtstylecontrolsqml, imtdesign) + Domain-specific GUI libraries
- **Documentation**: [Include/README.md - User Interface](Include/README.md#user-interface-16-libraries-total-7-core--9-domain-specific)

### 7. 3D & Visualization
2D/3D rendering, visualization, and hypermedia content.
- **6 libraries**: imt3d, imt3dgui, imt3dview, imthype, imthypegui, imtgeo
- **Documentation**: [Include/README.md - 3D & Visualization](Include/README.md#3d--visualization-6-libraries)

### 8. Content & Media
Document, image, and content management.
- **3 libraries**: imtimg, imtdoc, imtrepo
- **Documentation**: [Include/README.md - Content & Media](Include/README.md#content--media-3-libraries)

### 9. Communication & Integration
Network communication, email, and web services.
- **4 libraries**: imthttp, imtrest, imtmail, imtservice
- **Documentation**: [Include/README.md - Communication](Include/README.md#communication--integration-4-libraries), [Server Communication](#server-communication-and-connection-management) (below)

### 10. Security
Cryptography and encryption services.
- **1 library**: imtcrypt
- **Documentation**: [Include/README.md - Security](Include/README.md#security-1-library)

### 11. Code Generation & SDL
Schema-driven development and code generation tools.
- **5 libraries**: imtsdl, imtsdlgencpp, imtsdlgenqml, imtddl, imtoas
- **Documentation**: [SDL Documentation](#sdl-schema-documentation) (below), [Docs/SDL_Documentation_Index.md](Docs/SDL_Documentation_Index.md)

### 12. Development Tools
Developer utilities and integrations.
- **4 libraries**: imtdev, imtdevgui, imtpy, imtej
- **Documentation**: [Include/README.md - Development Tools](Include/README.md#development-tools-4-libraries)

### 13. Reporting & Logging
Logging, monitoring, and report generation.
- **4 libraries**: imtlog, imtloggui, imtreport, imtreportgui
- **Documentation**: [Include/README.md - Reporting & Logging](Include/README.md#reporting--logging-4-libraries)

### 14. Application Frameworks
High-level application scaffolding.
- **3 libraries**: imtapp, imtserverapp, imtupdate
- **Documentation**: [Include/README.md - Application Frameworks](Include/README.md#application-frameworks-3-libraries)

---

### SDL Schema Documentation

Complete documentation for SDL schema development in ImtCore, with special focus on the `ref` attribute for linking collections to GraphQL types.

**[📚 SDL Documentation Index](Docs/SDL_Documentation_Index.md)** - Complete overview and learning paths

#### Quick Links:

- **[Cheat Sheet](Docs/SDL_Ref_Cheat_Sheet.md)** (7KB) ⭐ **START HERE** - Quick reference card for daily development
- **[Quick Reference](Docs/SDL_Ref_Quick_Reference.md)** (3KB) - Brief syntax and rules
- **[Complete Guide](Docs/SDL_Reference_Attribute_Guide.md)** (21KB, 🇷🇺) - Comprehensive documentation with examples
- **[Practical Examples](Docs/SDL_Ref_Examples.md)** (23KB) - 5 real-world use cases
- **[Visual Diagrams](Docs/SDL_Ref_Diagrams.md)** (21KB) - 8 diagrams explaining workflows

**Total**: 70KB of documentation | 12 SDL examples | 8 visual diagrams | Russian + English

## Server Communication and Connection Management

ImtCore includes a comprehensive communication infrastructure for server connection configuration, multi-protocol dispatch, SSL/TLS security, and network connection health monitoring.

### Module

- **imtcom** - Server communication and connection management infrastructure

### Key Features

- **Multi-Protocol Support**: HTTP, WebSocket, gRPC, and File protocols
- **Server Connection Configuration**: Host, port, and protocol management via IServerConnectionInterface
- **Multi-Protocol Dispatch**: Server lifecycle management with protocol-specific control
- **SSL/TLS Security**: Comprehensive certificate and key management
- **Connection Monitoring**: Multiple health checking strategies (timer-based, GraphQL-based, internet availability)
- **Secure/Insecure Modes**: Connection flags for secure (HTTPS/WSS) and insecure (HTTP/WS) connections
- **ACF Component Architecture**: Factory-based instantiation with dependency injection
- **Observable Status**: IChangeable integration for reactive status updates

### Architecture

The module provides multi-protocol server infrastructure:

```
IServerConnectionInterface (configuration)
    ↓
IServerDispatcher (lifecycle management)
    ↓
Protocol-specific servers (HTTP, WebSocket)
    ↓
IConnectionStatusProvider (health monitoring)
```

### Core Interfaces

**IServerConnectionInterface:**
- Multi-protocol configuration (HTTP:9001, WebSocket:9000, gRPC:50101)
- CF_DEFAULT/CF_SECURE connection flags
- URL generation with automatic scheme selection

**IServerDispatcher:**
- Start/stop servers by protocol type
- Query server status (stopped, starting, running, stopping)
- Multi-protocol coordination

**IConnectionStatusProvider:**
- Observable connection status (CS_UNKNOWN, CS_DISCONNECTED, CS_CONNECTED)
- Three implementation strategies: async HTTP, GraphQL-based, internet availability

### Documentation

- **[Include/imtcom/README.md](Include/imtcom/README.md)** - Complete module documentation with 40+ usage examples
- **[Include/imtcom/imtcom.h](Include/imtcom/imtcom.h)** - Namespace-level Doxygen documentation

### Usage Example

```cpp
// Create connection with default ports
auto connection = icomp::CreateComponent<CServerConnectionInterfaceParamComp>();
connection->SetHost("api.example.com");
connection->SetConnectionFlags(IServerConnectionInterface::CF_SECURE);

// Generate protocol-specific URLs
QUrl httpUrl, wsUrl;
connection->GetUrl(IServerConnectionInterface::PT_HTTP, httpUrl);
// httpUrl = "https://api.example.com:9001"

connection->GetUrl(IServerConnectionInterface::PT_WEBSOCKET, wsUrl);
// wsUrl = "wss://api.example.com:9000"

// Dispatcher manages multiple protocols
auto dispatcher = icomp::CreateComponent<CServerDispatcherComp>();
dispatcher->StartServer(IServerConnectionInterface::PT_HTTP);
dispatcher->StartServer(IServerConnectionInterface::PT_WEBSOCKET);
```

For detailed documentation and API references, see [Include/imtcom/README.md](Include/imtcom/README.md).

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
