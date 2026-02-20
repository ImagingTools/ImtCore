# Software Bill of Materials (SBOM) Guide

## Overview

This document describes how to generate and use Software Bill of Materials (SBOM) for the ImtCore (ImagingTools Core Library) project to support EU Cyber Resilience Act (CRA) compliance.

## What is an SBOM?

A Software Bill of Materials (SBOM) is a comprehensive inventory of all components, libraries, and dependencies used in a software product. It helps with:

- **Transparency**: Understanding what's in your software
- **Security**: Identifying vulnerable components
- **Compliance**: Meeting regulatory requirements like the CRA
- **Risk Management**: Assessing supply chain risks

## Pre-Generated SBOM

ImtCore provides pre-generated SBOMs in both major formats:

### CycloneDX Format
A basic SBOM for ImtCore is provided in [`sbom.json`](sbom.json) in CycloneDX 1.5 format. This includes:

- ImtCore framework information
- Main runtime dependencies (Qt Framework, ACF, QuaZip, OpenSSL)
- License information
- Project metadata

### SPDX Format
An SPDX 2.3 format SBOM is provided in [`sbom.spdx.json`](sbom.spdx.json). This includes:

- ImtCore framework information
- Main runtime dependencies (Qt Framework, ACF, QuaZip, OpenSSL)
- License information (using SPDX license identifiers)
- Package relationships

**Important Notes**:
- The version in both SBOM files uses `1.0.0-dev` as a placeholder
- **Maintainers should update both files with each release** to reflect:
  - Current ImtCore version number
  - Creation timestamps (especially in SPDX format)
  - Document namespace UUID for SPDX (generate a new UUID for each release)
  - Specific Qt version used in testing
  - Specific ACF version dependency
  - Any dependency changes
- Both SBOMs should be regenerated or updated before each release

**Update Schedule**:
- Per Release: Update version numbers, timestamps, UUIDs, and dependency information
- As Needed: When dependencies are added, removed, or updated
- Quarterly: Verify all information is current

## SBOM Standards

ImtCore supports the following SBOM formats:

1. **CycloneDX** (Recommended)
   - Format: JSON or XML
   - Version: 1.5
   - Specification: https://cyclonedx.org/

2. **SPDX** (Alternative)
   - Format: JSON, YAML, or RDF
   - Version: 2.3
   - Specification: https://spdx.dev/

## Generating SBOM for Your Project

If you're using ImtCore in your project, you need to generate an SBOM that includes ImtCore and all its dependencies.

### Method 1: Using CycloneDX Tools

#### For CMake Projects

1. Install CycloneDX CMake plugin:
```bash
# Add to your CMakeLists.txt
find_package(CycloneDX QUIET)
if(CycloneDX_FOUND)
    cyclonedx_generate_sbom(TARGET your_target)
endif()
```

2. Generate SBOM:
```bash
cmake --build . --target cyclonedx-sbom
```

#### For C++ Projects with Conan

```bash
# Install the plugin
pip install cyclonedx-conan

# Generate SBOM
conan install . --install-folder=build
cyclonedx-conan sbom -o sbom.json
```

### Method 2: Using SPDX Tools

#### Using spdx-sbom-generator

```bash
# Install the tool
go install github.com/opensbom-generator/spdx-sbom-generator@latest

# Generate SBOM
spdx-sbom-generator
```

### Method 3: Manual SBOM Creation

For custom setups, you can manually create an SBOM:

1. Start with the ImtCore SBOM template: [`sbom.json`](sbom.json)
2. Add your project metadata
3. Include all dependencies with:
   - Name and version
   - License information
   - Download location or package URL
   - Checksums (SHA-256 recommended)
4. Document the dependency tree

Example structure:
```json
{
  "bomFormat": "CycloneDX",
  "specVersion": "1.5",
  "metadata": {
    "component": {
      "type": "application",
      "name": "YourApp",
      "version": "1.0.0"
    }
  },
  "components": [
    {
      "type": "library",
      "name": "ImtCore",
      "version": "X.Y.Z",
      "purl": "pkg:github/ImagingTools/ImtCore@X.Y.Z",
      "licenses": [{"license": {"id": "LGPL-2.1-or-later"}}]
    },
    {
      "type": "library",
      "name": "ACF",
      "version": "X.Y.Z",
      "purl": "pkg:github/ImagingTools/Acf@X.Y.Z",
      "licenses": [{"license": {"id": "LGPL-2.1-or-later"}}]
    },
    {
      "type": "library",
      "name": "Qt",
      "version": "6.8"
    }
  ]
}
```

## SBOM Components for ImtCore

### Core Dependencies

**ACF (Application Component Framework) (Required)**
- Component: ACF
- Version: 1.x (check conanfile.py for exact version)
- License: LGPL-2.1-or-later / GPL-2.0-or-later / GPL-3.0-or-later / Commercial
- PURL: pkg:github/ImagingTools/Acf
- Website: https://github.com/ImagingTools/Acf
- Purpose: Application component framework that ImtCore is built upon

**Qt Framework (Required)**
- Component: Qt
- Version: 6.8.3 (configurable)
- License: LGPL-3.0 / GPL-2.0 / GPL-3.0 / Commercial
- PURL: pkg:generic/qt@6.8.3 (example)
- Website: https://www.qt.io/
- Purpose: Cross-platform application framework

**QuaZip (Required)**
- Component: QuaZip
- Version: 1.x
- License: LGPL-2.1-or-later
- Purpose: Qt/C++ wrapper for ZIP/UNZIP package

**OpenSSL (Required)**
- Component: OpenSSL
- Version: 1.1.x
- License: Apache-2.0
- PURL: pkg:generic/openssl@1.1
- Website: https://www.openssl.org/
- Purpose: Cryptography and SSL/TLS toolkit

**Zlib (Required)**
- Component: Zlib
- Version: 1.2.11
- License: Zlib
- Purpose: Compression library

**Standard C++ Library (Required)**
- Component: Implementation-dependent (libstdc++, libc++, MSVC STL)
- License: Implementation-dependent
- Purpose: C++ standard library functionality

### Build-Time Dependencies

**CMake (Optional - for CMake builds)**
- Version: 3.x or later
- License: BSD-3-Clause
- PURL: pkg:generic/cmake
- Purpose: Build system generator

**Ninja (Optional - for Ninja builds)**
- License: Apache-2.0
- Purpose: Build system

**Conan (Optional - for Conan package management)**
- Version: 1.x or 2.x
- License: MIT
- Purpose: C/C++ package manager

**Doxygen (Optional - for documentation)**
- License: GPL-2.0
- Purpose: API documentation generation

**patchelf (Linux only)**
- Version: 0.13
- License: GPL-3.0
- Purpose: Modifying ELF binaries

### Development Dependencies

When developing or building from source, additional tools may be needed:
- C++ compiler (GCC, Clang, MSVC) with C++17 support
- Git (for source control)
- Git LFS (for large binary files)
- Python 3.8+ (for build scripts)
- Graphviz (for Doxygen diagrams)

## ImtCore Libraries and Modules

The ImtCore framework consists of multiple libraries. Include relevant ones in your SBOM:

**Core Libraries:**
- `imtcore` - Core utilities and framework
- `imtbase` - Base components
- `imtcom` - Component framework extensions
- `imttest` - Testing framework
- `imtlog` - Logging framework
- `imtloggui` - Logging GUI components

**Data and File Handling:**
- `imtfile` - File handling utilities
- `imtcol` - Collection utilities
- `imtcrypt` - Cryptography utilities
- `imtzip` - ZIP file handling
- `imtimg` - Image processing

**Application Framework:**
- `imtapp` - Application framework
- `imtdoc` - Document framework
- `imtgui` - GUI utilities
- `imtwidgets` - Widget components
- `imtstyle` - Style system
- `imtstylecontrolsqml` - QML style controls

**3D Graphics:**
- `imt3d` - 3D graphics utilities
- `imt3dgui` - 3D GUI components
- `imt3dview` - 3D view framework

**Database and Persistence:**
- `imtdb` - Database utilities
- `imtdbgui` - Database GUI components
- `imtdbgql` - GraphQL database integration
- `imtmdbx` - MDBX database support
- `imtmongo` - MongoDB integration

**Authentication and Authorization:**
- `imtauth` - Authentication framework
- `imtauthdb` - Authentication database
- `imtauthgui` - Authentication GUI
- `imtauthgql` - Authentication GraphQL

**Network and Communication:**
- `imthttp` - HTTP utilities
- `imtrest` - REST API support
- `imtgql` - GraphQL support
- `imtgqltest` - GraphQL testing
- `imtmail` - Email utilities
- `imtclientgql` - GraphQL client
- `imtservergql` - GraphQL server
- `imtserverapp` - Server application framework
- `imtservice` - Service framework

**Qt Integration:**
- `imtqml` - QML integration
- `imtqml2d` - QML 2D graphics
- `imtguigql` - GUI GraphQL integration

**Design and Development:**
- `imtdesign` - Design framework
- `imtdev` - Development utilities
- `imtdevgui` - Development GUI tools
- `imtddl` - Data Definition Language
- `imtsdl` - Service Definition Language
- `imtsdlgencpp` - SDL C++ code generator
- `imtsdlgenqml` - SDL QML code generator

**Licensing:**
- `imtlic` - Licensing framework
- `imtlicdb` - Licensing database
- `imtlicgui` - Licensing GUI
- `imtlicgql` - Licensing GraphQL

**Other:**
- `imtpy` - Python integration
- `imtrepo` - Repository utilities
- `imtreport` - Reporting framework
- `imtreportgui` - Reporting GUI
- `imtupdate` - Update framework
- `imtoas` - OpenAPI Specification support
- `imtgeo` - Geographic utilities
- `imthype` - Hypertext processing
- `imthypedb` - Hypertext database
- `imthypegui` - Hypertext GUI
- `imtej` - EJ integration

## Updating the SBOM

### When to Update

Update your SBOM when:
- Adding or removing dependencies
- Upgrading dependency versions
- Changing build configurations
- Discovering security vulnerabilities in dependencies
- Releasing a new version of your software

### Update Process

1. Review all dependencies
2. Update version numbers
3. Verify license information
4. Add/remove components as needed
5. Update metadata (timestamp, version)
6. Validate SBOM format
7. Commit to version control
8. Include in release artifacts

## Validating SBOM

### Basic JSON Validation

#### CycloneDX Format (sbom.json)

```bash
# Validate JSON syntax
jq empty sbom.json

# Validate CycloneDX structure
jq -e '.bomFormat == "CycloneDX"' sbom.json
jq -e '.specVersion' sbom.json
jq -e '.metadata' sbom.json
jq -e '.components' sbom.json
```

#### SPDX Format (sbom.spdx.json)

```bash
# Validate JSON syntax
jq empty sbom.spdx.json

# Validate SPDX structure
jq -e '.spdxVersion == "SPDX-2.3"' sbom.spdx.json
jq -e '.dataLicense' sbom.spdx.json
jq -e '.creationInfo' sbom.spdx.json
jq -e '.packages' sbom.spdx.json
```

### CycloneDX Validation (Optional)

Note: The `@cyclonedx/cyclonedx-cli` npm package is not currently available. For full schema validation, you can use alternative tools:

```bash
# Using Python cyclonedx-lib (if installed)
pip install cyclonedx-bom
# Then validate with custom script

# Or use online validators
# Visit: https://cyclonedx.org/
```

### SPDX Validation

```bash
# Install tools
pip install spdx-tools

# Validate SBOM
pyspdxtools -i sbom.spdx.json
```

## SBOM Best Practices

1. **Completeness**: Include all dependencies, including transitive ones
2. **Accuracy**: Verify version numbers and licenses
3. **Automation**: Use tools to generate SBOMs automatically
4. **Version Control**: Track SBOM changes in Git
5. **Distribution**: Include SBOM with releases
6. **Updates**: Keep SBOM current with dependencies
7. **Verification**: Validate SBOM format and content
8. **Documentation**: Document how to generate and use SBOM

## SBOM and Security

### Vulnerability Scanning

Use SBOM to scan for vulnerabilities:

```bash
# Using OWASP Dependency-Check
dependency-check --scan . --format JSON --enableExperimental

# Using Grype
grype sbom:sbom.json

# Using Trivy
trivy sbom sbom.json
```

### Monitoring

- Set up automated scanning in CI/CD
- Subscribe to security advisories for dependencies
- Use tools like GitHub Dependabot
- Review SBOM regularly for outdated components

## CRA Compliance

For EU Cyber Resilience Act compliance:

1. **Include SBOM in Product Documentation**
   - Make SBOM available to users
   - Update SBOM with each release
   - Document SBOM format and location

2. **Vulnerability Management**
   - Use SBOM for vulnerability tracking
   - Document known vulnerabilities
   - Provide remediation guidance

3. **Transparency**
   - Publish SBOM in accessible format
   - Include license information
   - Document component relationships

See [CRA_COMPLIANCE.md](CRA_COMPLIANCE.md) for complete CRA compliance information.

## Example SBOMs

### Minimal Application Using ImtCore (CycloneDX Format)

```json
{
  "bomFormat": "CycloneDX",
  "specVersion": "1.5",
  "metadata": {
    "component": {
      "type": "application",
      "name": "MyApp",
      "version": "1.0.0"
    }
  },
  "components": [
    {
      "type": "library",
      "name": "ImtCore",
      "version": "1.0.0",
      "purl": "pkg:github/ImagingTools/ImtCore@1.0.0",
      "licenses": [
        {"license": {"id": "LGPL-2.1-or-later"}},
        {"expression": "LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR Commercial"}
      ]
    },
    {
      "type": "library",
      "name": "ACF",
      "version": "1.0.0",
      "purl": "pkg:github/ImagingTools/Acf@1.0.0",
      "licenses": [
        {"license": {"id": "LGPL-2.1-or-later"}},
        {"expression": "LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR Commercial"}
      ]
    },
    {
      "type": "library",
      "name": "Qt",
      "version": "6.8.3",
      "licenses": [{"expression": "LGPL-3.0-only OR GPL-2.0-only"}]
    },
    {
      "type": "library",
      "name": "QuaZip",
      "version": "1.4",
      "licenses": [{"license": {"id": "LGPL-2.1-or-later"}}]
    },
    {
      "type": "library",
      "name": "OpenSSL",
      "version": "1.1.1",
      "licenses": [{"license": {"id": "Apache-2.0"}}]
    }
  ]
}
```

### Minimal Application Using ImtCore (SPDX Format)

```json
{
  "spdxVersion": "SPDX-2.3",
  "dataLicense": "CC0-1.0",
  "SPDXID": "SPDXRef-DOCUMENT",
  "name": "MyApp-1.0.0",
  "documentNamespace": "https://example.com/myapp/spdxdocs/sbom-1.0.0-550e8400-e29b-41d4-a716-446655440000",
  "creationInfo": {
    "created": "2026-01-18T10:00:00Z",
    "creators": ["Tool: MyApp SBOM Generator"]
  },
  "hasExtractedLicensingInfos": [
    {
      "licenseId": "LicenseRef-Qt-Commercial",
      "extractedText": "Qt is available under commercial license agreements",
      "name": "Qt Commercial License",
      "seeAlsos": ["https://www.qt.io/licensing/"]
    },
    {
      "licenseId": "LicenseRef-ImtCore-Commercial",
      "extractedText": "ImtCore is available under commercial license agreements",
      "name": "ImtCore Commercial License",
      "seeAlsos": ["https://github.com/ImagingTools/ImtCore"]
    },
    {
      "licenseId": "LicenseRef-ACF-Commercial",
      "extractedText": "ACF is available under commercial license agreements",
      "name": "ACF Commercial License",
      "seeAlsos": ["https://github.com/ImagingTools/Acf"]
    }
  ],
  "packages": [
    {
      "SPDXID": "SPDXRef-Package-MyApp",
      "name": "MyApp",
      "versionInfo": "1.0.0",
      "downloadLocation": "NOASSERTION",
      "filesAnalyzed": false,
      "licenseConcluded": "NOASSERTION",
      "licenseDeclared": "NOASSERTION"
    },
    {
      "SPDXID": "SPDXRef-Package-ImtCore",
      "name": "ImtCore",
      "versionInfo": "1.0.0",
      "downloadLocation": "https://github.com/ImagingTools/ImtCore",
      "filesAnalyzed": false,
      "licenseConcluded": "(LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial)",
      "licenseDeclared": "(LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial)"
    },
    {
      "SPDXID": "SPDXRef-Package-ACF",
      "name": "ACF",
      "versionInfo": "1.0.0",
      "downloadLocation": "https://github.com/ImagingTools/Acf",
      "filesAnalyzed": false,
      "licenseConcluded": "(LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ACF-Commercial)",
      "licenseDeclared": "(LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ACF-Commercial)"
    },
    {
      "SPDXID": "SPDXRef-Package-Qt",
      "name": "Qt",
      "versionInfo": "6.8.3",
      "downloadLocation": "https://www.qt.io/",
      "filesAnalyzed": false,
      "licenseConcluded": "(LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only OR LicenseRef-Qt-Commercial)",
      "licenseDeclared": "(LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only OR LicenseRef-Qt-Commercial)",
      "comment": "Qt is available under multiple licenses including open source (LGPL/GPL) and commercial options"
    },
    {
      "SPDXID": "SPDXRef-Package-QuaZip",
      "name": "QuaZip",
      "versionInfo": "1.4",
      "downloadLocation": "https://github.com/stachenov/quazip",
      "filesAnalyzed": false,
      "licenseConcluded": "LGPL-2.1-or-later",
      "licenseDeclared": "LGPL-2.1-or-later"
    },
    {
      "SPDXID": "SPDXRef-Package-OpenSSL",
      "name": "OpenSSL",
      "versionInfo": "1.1.1",
      "downloadLocation": "https://www.openssl.org/",
      "filesAnalyzed": false,
      "licenseConcluded": "Apache-2.0",
      "licenseDeclared": "Apache-2.0"
    }
  ],
  "relationships": [
    {
      "spdxElementId": "SPDXRef-DOCUMENT",
      "relationshipType": "DESCRIBES",
      "relatedSpdxElement": "SPDXRef-Package-MyApp"
    },
    {
      "spdxElementId": "SPDXRef-Package-MyApp",
      "relationshipType": "DEPENDS_ON",
      "relatedSpdxElement": "SPDXRef-Package-ImtCore"
    },
    {
      "spdxElementId": "SPDXRef-Package-MyApp",
      "relationshipType": "DEPENDS_ON",
      "relatedSpdxElement": "SPDXRef-Package-Qt"
    },
    {
      "spdxElementId": "SPDXRef-Package-ImtCore",
      "relationshipType": "DEPENDS_ON",
      "relatedSpdxElement": "SPDXRef-Package-ACF"
    },
    {
      "spdxElementId": "SPDXRef-Package-ImtCore",
      "relationshipType": "DEPENDS_ON",
      "relatedSpdxElement": "SPDXRef-Package-Qt"
    },
    {
      "spdxElementId": "SPDXRef-Package-ImtCore",
      "relationshipType": "DEPENDS_ON",
      "relatedSpdxElement": "SPDXRef-Package-QuaZip"
    },
    {
      "spdxElementId": "SPDXRef-Package-ImtCore",
      "relationshipType": "DEPENDS_ON",
      "relatedSpdxElement": "SPDXRef-Package-OpenSSL"
    }
  ]
}
```

## Resources

- [CycloneDX Specification](https://cyclonedx.org/)
- [SPDX Specification](https://spdx.dev/)
- [NTIA SBOM Minimum Elements](https://www.ntia.gov/sbom)
- [ImtCore Documentation](https://imagingtools.github.io/ImtCore/)
- [ACF Documentation](https://imagingtools.github.io/Acf/)
- [ImtCore Security Policy](SECURITY.md)
- [CRA Compliance](CRA_COMPLIANCE.md)

## Support

For questions about SBOM generation for ImtCore:

- Check the [documentation](https://imagingtools.github.io/ImtCore/)
- Open an [issue](https://github.com/ImagingTools/ImtCore/issues)
- Review [CRA compliance documentation](CRA_COMPLIANCE.md)

**Last Updated**: January 2026
