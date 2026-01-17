# ImtCore Library Reorganization Proposal

**Date:** January 2026  
**Author:** Code Analysis  
**Status:** Proposal

## Executive Summary

This document proposes a reorganization of the ImtCore library structure to improve discoverability, maintainability, and logical grouping. The current flat structure contains 61 libraries in a single directory, making it challenging to understand relationships and navigate the codebase.

## Current State Analysis

### Current Structure
The `Include/` folder contains 61 libraries organized in a flat structure:
- All libraries share the `imt` prefix
- Suffixes indicate technical layers: `gui`, `gql`, `db`, `qml`
- No hierarchical organization
- No clear separation between domains and technical concerns

### Identified Patterns

#### 1. **Suffix-Based Technical Layers**
- `*gui` - Qt-based user interface components (10 libraries)
- `*gql` - GraphQL API endpoints (8 libraries)
- `*db` - Database persistence layers (2 libraries)
- `*qml` - QML/JavaScript UI components (5 libraries)
- `gen*` - Code generators (3 libraries)

#### 2. **Domain Areas** (no suffix)
- **Core Infrastructure**: base, core, com, col, test
- **Authentication & Authorization**: auth
- **Data Management**: db, file, doc, repo
- **Security**: crypt, lic
- **Communication**: http, rest, mail, service
- **Content & Media**: img, 3d, geo, hype
- **Development Tools**: dev, ddl, sdl, oas, design, py
- **Application Frameworks**: app, serverapp, update, log

## Problems with Current Structure

### 1. **Discoverability Issues**
- Difficult to find related libraries (e.g., auth, authdb, authgql, authgui are separate)
- No clear entry points for developers
- Unclear which libraries are foundational vs. specialized

### 2. **Maintenance Challenges**
- No visual grouping of related code
- Hard to understand dependencies between layers
- Inconsistent naming (e.g., `imtmdbx`, `imtmongo` vs `imtdb`)

### 3. **Naming Inconsistencies**
- Mix of abbreviations and full names (`img` vs `image`, `lic` vs `license`)
- Unclear purpose from names alone (`ej`, `oas`, `ddl`, `hype`)
- Some names conflict with common terms (`service`, `test`)

### 4. **Scalability Concerns**
- Flat structure doesn't scale well beyond 60+ libraries
- Adding new features creates more clutter
- No clear pattern for where to add new code

## Proposed Reorganization

### Option A: Hierarchical by Domain (Recommended)

Organize libraries by business domain with subdirectories for technical layers:

```
Include/
├── core/                          # Foundation libraries
│   ├── base/                      # imtbase → core/base
│   ├── core/                      # imtcore → core/core
│   ├── collections/               # imtcol → core/collections
│   ├── communication/             # imtcom → core/communication
│   └── testing/                   # imttest → core/testing
│
├── auth/                          # Authentication & Authorization
│   ├── core/                      # imtauth → auth/core
│   ├── db/                        # imtauthdb → auth/db
│   ├── gql/                       # imtauthgql → auth/gql
│   └── gui/                       # imtauthgui → auth/gui
│
├── database/                      # Data persistence
│   ├── core/                      # imtdb → database/core
│   ├── gql/                       # imtdbgql → database/gql
│   ├── gui/                       # imtdbgui → database/gui
│   ├── mdbx/                      # imtmdbx → database/mdbx
│   └── mongodb/                   # imtmongo → database/mongodb
│
├── licensing/                     # License management
│   ├── core/                      # imtlic → licensing/core
│   ├── db/                        # imtlicdb → licensing/db
│   ├── gql/                       # imtlicgql → licensing/gql
│   └── gui/                       # imtlicgui → licensing/gui
│
├── content/                       # Content & media handling
│   ├── files/                     # imtfile → content/files
│   ├── images/                    # imtimg → content/images
│   ├── documents/                 # imtdoc → content/documents
│   ├── compression/               # imtzip → content/compression
│   └── repositories/              # imtrepo → content/repositories
│
├── visualization/                 # 2D/3D rendering
│   ├── 3d/
│   │   ├── core/                  # imt3d → visualization/3d/core
│   │   ├── gui/                   # imt3dgui → visualization/3d/gui
│   │   └── view/                  # imt3dview → visualization/3d/view
│   ├── hypermedia/
│   │   ├── core/                  # imthype → visualization/hypermedia/core
│   │   └── gui/                   # imthypegui → visualization/hypermedia/gui
│   └── geospatial/                # imtgeo → visualization/geospatial
│
├── api/                           # API layers
│   ├── graphql/
│   │   ├── core/                  # imtgql → api/graphql/core
│   │   ├── client/                # imtclientgql → api/graphql/client
│   │   ├── server/                # imtservergql → api/graphql/server
│   │   ├── gui/                   # imtguigql → api/graphql/gui
│   │   └── testing/               # imtgqltest → api/graphql/testing
│   ├── rest/                      # imtrest → api/rest
│   ├── http/                      # imthttp → api/http
│   └── openapi/                   # imtoas → api/openapi
│
├── security/                      # Security & encryption
│   └── cryptography/              # imtcrypt → security/cryptography
│
├── communication/                 # External communication
│   ├── email/                     # imtmail → communication/email
│   └── service/                   # imtservice → communication/service
│
├── ui/                           # User interface components
│   ├── core/                      # imtgui → ui/core
│   ├── widgets/                   # imtwidgets → ui/widgets
│   ├── qml/
│   │   ├── core/                  # imtqml → ui/qml/core
│   │   ├── 2d/                    # imtqml2d → ui/qml/2d
│   │   └── controls/              # imtstylecontrolsqml → ui/qml/controls
│   ├── styling/                   # imtstyle → ui/styling
│   └── design-system/             # imtdesign → ui/design-system
│
├── reporting/                     # Reporting & logging
│   ├── reports/
│   │   ├── core/                  # imtreport → reporting/reports/core
│   │   └── gui/                   # imtreportgui → reporting/reports/gui
│   └── logging/
│       ├── core/                  # imtlog → reporting/logging/core
│       └── gui/                   # imtloggui → reporting/logging/gui
│
├── codegen/                       # Code generation tools
│   ├── sdl/
│   │   ├── core/                  # imtsdl → codegen/sdl/core
│   │   ├── cpp/                   # imtsdlgencpp → codegen/sdl/cpp
│   │   └── qml/                   # imtsdlgenqml → codegen/sdl/qml
│   └── ddl/                       # imtddl → codegen/ddl
│
├── development/                   # Developer tools
│   ├── core/                      # imtdev → development/core
│   ├── gui/                       # imtdevgui → development/gui
│   ├── python/                    # imtpy → development/python
│   └── export/                    # imtej → development/export
│
└── application/                   # Application frameworks
    ├── desktop/                   # imtapp → application/desktop
    ├── server/                    # imtserverapp → application/server
    └── updates/                   # imtupdate → application/updates
```

### Option B: Hybrid Approach (Layer + Domain)

Organize by technical layer first, then by domain:

```
Include/
├── core/                          # Core libraries (no dependencies)
│   ├── base/
│   ├── core/
│   ├── collections/
│   └── testing/
│
├── infrastructure/                # Infrastructure services
│   ├── database/
│   ├── communication/
│   ├── security/
│   └── logging/
│
├── domain/                        # Business domains
│   ├── auth/
│   ├── licensing/
│   ├── content/
│   └── visualization/
│
├── api/                          # API layers
│   ├── graphql/
│   ├── rest/
│   └── http/
│
├── ui/                           # User interfaces
│   ├── qt/
│   └── qml/
│
└── tools/                        # Development tools
    ├── codegen/
    └── development/
```

## Detailed Recommendations

### 1. Naming Conventions

#### Current Issues
- Inconsistent abbreviations (img/image, lic/license, auth/authentication)
- Unclear meanings (ej, oas, hype, ddl)
- Redundant prefixes (all start with `imt`)

#### Proposed Standards

**Directory Names:**
- Use full, descriptive names: `images` not `img`, `licensing` not `lic`
- Avoid cryptic abbreviations
- Use kebab-case for multi-word names: `design-system`, `open-api`

**Library/Namespace Names:**
- Keep `imt` prefix for C++ namespaces/libraries to avoid conflicts
- Directory structure provides context, names can be simpler
- Technical layer indicators (`gui`, `gql`, `db`) remain clear

**Examples:**
```
Current:            Proposed Directory:        Library Name:
imtimg           →  content/images/         →  imtimg or imt::images
imtlic           →  licensing/core/         →  imtlic or imt::licensing
imtej            →  development/export/     →  imtej or imt::export
imtoas           →  api/openapi/            →  imtoas or imt::openapi
imthype          →  visualization/hypermedia/ → imthype or imt::hypermedia
```

### 2. Migration Strategy

#### Phase 1: Documentation & Preparation (Week 1-2)
1. Document all inter-library dependencies
2. Create migration scripts for automated file moves
3. Update build system (CMake) to support both old and new paths
4. Communicate changes to all developers

#### Phase 2: Gradual Migration (Week 3-6)
1. Create new directory structure
2. Add symbolic links from old to new locations
3. Migrate one domain at a time (start with smallest)
4. Update imports/includes incrementally
5. Test after each domain migration

#### Phase 3: Cleanup (Week 7-8)
1. Remove symbolic links
2. Remove old directory structure
3. Update all documentation
4. Final testing and verification

#### Phase 4: Completion
1. Update developer onboarding docs
2. Archive this migration document
3. Establish guidelines for future additions

### 3. Build System Updates

#### CMake Changes Required
```cmake
# Old approach
include_directories(${IMTCORE_DIR}/Include/imtauth)
include_directories(${IMTCORE_DIR}/Include/imtauthgui)

# New approach (Option A)
include_directories(${IMTCORE_DIR}/Include/auth/core)
include_directories(${IMTCORE_DIR}/Include/auth/gui)

# Or with helper macro
imt_include_domain(auth)  # Includes all auth/* subdirectories
```

#### Backward Compatibility
```cmake
# Compatibility layer during transition
if(EXISTS ${IMTCORE_DIR}/Include/auth/core)
    set(IMT_NEW_STRUCTURE ON)
    set(IMTAUTH_INCLUDE_DIR ${IMTCORE_DIR}/Include/auth/core)
else()
    set(IMTAUTH_INCLUDE_DIR ${IMTCORE_DIR}/Include/imtauth)
endif()
```

### 4. Code Structure Best Practices

#### Grouping Principles
1. **Cohesion**: Related functionality stays together
2. **Low Coupling**: Clear boundaries between domains
3. **Layering**: Dependencies flow from core → domain → api → ui
4. **Discoverability**: Structure matches mental models

#### Technical Layer Organization
Each domain follows consistent structure:
```
domain/
├── core/           # Domain logic (no UI/API dependencies)
├── db/             # Persistence layer
├── gql/            # GraphQL API
├── gui/            # Qt GUI components
└── qml/            # QML UI components (if needed)
```

### 5. Documentation Requirements

#### Per-Library Documentation
Each library directory should contain:
- `README.md` - Purpose, dependencies, usage examples
- `CHANGELOG.md` - Version history
- `CMakeLists.txt` - Build configuration

#### Domain Documentation
Each domain folder should contain:
- `README.md` - Overview of domain and its libraries
- Architecture diagrams showing layer dependencies
- Getting started guide

### 6. Transition Risks & Mitigations

| Risk | Impact | Mitigation |
|------|--------|------------|
| Build breaks | High | Maintain backward compatibility layer for 2 release cycles |
| Developer confusion | Medium | Comprehensive docs, team training sessions |
| External dependencies | Medium | Document all public APIs, provide migration guide |
| Git history loss | Low | Use `git mv` to preserve history |
| Merge conflicts | High | Coordinate with team, minimize parallel work during migration |

## Comparison of Options

| Criteria | Option A (Domain-First) | Option B (Layer-First) |
|----------|------------------------|------------------------|
| **Discoverability** | ⭐⭐⭐⭐⭐ Excellent - related features grouped | ⭐⭐⭐ Good - but need to search multiple sections |
| **Maintenance** | ⭐⭐⭐⭐⭐ All auth code together | ⭐⭐⭐ Code scattered by layer |
| **Feature Teams** | ⭐⭐⭐⭐⭐ Perfect for domain-driven teams | ⭐⭐⭐ Better for platform teams |
| **Dependency Management** | ⭐⭐⭐⭐ Clear within domains | ⭐⭐⭐⭐⭐ Clear across layers |
| **Learning Curve** | ⭐⭐⭐⭐ Intuitive for domain work | ⭐⭐⭐ Requires understanding of both axes |
| **Scalability** | ⭐⭐⭐⭐⭐ Grows naturally with features | ⭐⭐⭐⭐ Good but more directories at top |

## Recommendation

**Adopt Option A: Hierarchical by Domain**

### Rationale
1. **Developer Workflow**: Most work happens within a single domain (e.g., working on auth features touches auth/core, auth/db, auth/gql, auth/gui)
2. **Code Ownership**: Teams can own entire domains
3. **Reduced Cognitive Load**: Finding "all auth code" is immediate
4. **Industry Standard**: Matches common microservices and domain-driven design patterns
5. **Better IDE Experience**: Collapsible domain folders in IDEs

### Implementation Priority

**Phase 1 - High Impact, Low Risk:**
1. `auth/` - Clear domain boundaries
2. `licensing/` - Self-contained
3. `reporting/` - Limited dependencies

**Phase 2 - Medium Impact:**
4. `database/` - Core infrastructure
5. `visualization/` - Isolated domain
6. `api/` - Clear technical grouping

**Phase 3 - Complex (many dependencies):**
7. `ui/` - Many cross-cutting concerns
8. `core/` - Base dependencies
9. `application/` - Top-level frameworks

## Quick Wins (Immediate Actions)

While planning the full reorganization, these immediate improvements can be made:

### 1. Add README Files
Create `README.md` in each library explaining:
- Purpose and responsibilities
- Dependencies
- Key classes/components
- Usage examples

### 2. Create Index Documentation
Add `Include/README.md` with:
- Overview of all libraries
- Dependency diagram
- Naming conventions explanation
- How to choose the right library

### 3. Improve Library Names (No File Moves)
Document clearer names/aliases:
- `imtej` → Export/JSON utilities
- `imtoas` → OpenAPI Schema tools
- `imthype` → Hypermedia rendering
- `imtddl` → Data Definition Language

### 4. Establish Naming Guidelines
Document for future libraries:
- When to create a new library
- Naming conventions
- Required structure (`core` → `db` → `gql` → `gui`)
- Dependency rules

## Metrics for Success

Track these metrics before and after reorganization:

1. **Time to Find Code**: Developer survey on "time to locate feature X"
2. **Build Time**: Ensure no regression in build performance
3. **Onboarding Time**: Time for new developers to understand structure
4. **Maintenance Cost**: Time to make cross-cutting changes
5. **Code Reuse**: Tracking of duplicate code across domains

## Conclusion

The current flat structure of 61 libraries has served its purpose but has reached its scalability limits. A domain-first hierarchical organization will:

- Improve code discoverability
- Support feature team ownership
- Reduce cognitive load
- Scale better as the codebase grows
- Align with modern software architecture patterns

**Recommended Action**: Begin with documentation improvements (Quick Wins) while planning the full migration to Option A structure.

## Appendices

### Appendix A: Complete Library Mapping

| Current Library | New Location (Option A) | Purpose |
|----------------|------------------------|---------|
| imt3d | visualization/3d/core | 3D engine core |
| imt3dgui | visualization/3d/gui | 3D GUI components |
| imt3dview | visualization/3d/view | 3D viewport |
| imtapp | application/desktop | Desktop app framework |
| imtauth | auth/core | Authentication core |
| imtauthdb | auth/db | Auth persistence |
| imtauthgql | auth/gql | Auth GraphQL API |
| imtauthgui | auth/gui | Auth UI components |
| imtbase | core/base | Base framework |
| imtclientgql | api/graphql/client | GraphQL client |
| imtcol | core/collections | Collection utilities |
| imtcom | core/communication | Communication protocols |
| imtcore | core/core | Core version info |
| imtcrypt | security/cryptography | Encryption services |
| imtdb | database/core | SQL database core |
| imtdbgql | database/gql | Database GraphQL API |
| imtdbgui | database/gui | Database UI |
| imtddl | codegen/ddl | Data definition lang |
| imtdesign | ui/design-system | Design tokens |
| imtdev | development/core | Dev tools core |
| imtdevgui | development/gui | Dev tools UI |
| imtdoc | content/documents | Document handling |
| imtej | development/export | Export/JSON utilities |
| imtfile | content/files | File I/O |
| imtgeo | visualization/geospatial | Geographic data |
| imtgql | api/graphql/core | GraphQL core |
| imtgqltest | api/graphql/testing | GraphQL tests |
| imtgui | ui/core | GUI core widgets |
| imtguigql | api/graphql/gui | GraphQL UI integration |
| imthttp | api/http | HTTP client/server |
| imthype | visualization/hypermedia | Hypermedia rendering |
| imthypegui | visualization/hypermedia/gui | Hypermedia UI |
| imtimg | content/images | Image processing |
| imtlic | licensing/core | License management |
| imtlicdb | licensing/db | License persistence |
| imtlicgql | licensing/gql | License GraphQL API |
| imtlicgui | licensing/gui | License UI |
| imtlog | reporting/logging/core | Logging framework |
| imtloggui | reporting/logging/gui | Logging UI |
| imtmail | communication/email | Email/SMTP |
| imtmdbx | database/mdbx | MDBX database |
| imtmongo | database/mongodb | MongoDB integration |
| imtoas | api/openapi | OpenAPI schema |
| imtpy | development/python | Python bindings |
| imtqml | ui/qml/core | QML core utilities |
| imtqml2d | ui/qml/2d | QML 2D graphics |
| imtrepo | content/repositories | Repository management |
| imtreport | reporting/reports/core | Reporting engine |
| imtreportgui | reporting/reports/gui | Report UI |
| imtrest | api/rest | REST API framework |
| imtsdl | codegen/sdl/core | Schema definition lang |
| imtsdlgencpp | codegen/sdl/cpp | SDL to C++ generator |
| imtsdlgenqml | codegen/sdl/qml | SDL to QML generator |
| imtserverapp | application/server | Server app framework |
| imtservergql | api/graphql/server | GraphQL server |
| imtservice | communication/service | Service framework |
| imtstyle | ui/styling | Style/theme system |
| imtstylecontrolsqml | ui/qml/controls | QML style controls |
| imttest | core/testing | Testing utilities |
| imtupdate | application/updates | Update management |
| imtwidgets | ui/widgets | UI widget library |
| imtzip | content/compression | Compression/ZIP |

### Appendix B: Dependency Analysis

Key dependency chains (from bottom to top):
```
Layer 0 (No dependencies):
  - core/core, core/base, core/testing

Layer 1 (Core only):
  - core/collections, core/communication
  - security/cryptography
  - content/files, content/compression

Layer 2 (Infrastructure):
  - database/core, database/mdbx, database/mongodb
  - api/http, communication/email
  
Layer 3 (Domain Logic):
  - auth/core, licensing/core
  - content/images, content/documents
  - visualization/3d/core

Layer 4 (Persistence):
  - auth/db, licensing/db
  - database/gql

Layer 5 (APIs):
  - auth/gql, licensing/gql
  - api/graphql/server, api/rest

Layer 6 (UI):
  - auth/gui, licensing/gui
  - ui/core, ui/widgets, ui/qml/core

Layer 7 (Applications):
  - application/desktop, application/server
```

### Appendix C: External Interface Compatibility

**Public Headers**: These headers are likely used by external projects:
- `Include/ImtCore.h` - Main include (needs updating)
- All library exports (COM interfaces)
- Public API headers in `gql/` and `rest/` libraries

**Compatibility Strategy**:
1. Keep `ImtCore.h` at top level, update includes
2. Provide compatibility headers at old locations (deprecated)
3. Use `#pragma message` to warn about deprecated includes
4. Remove compatibility layer after 2 major versions

### Appendix D: Tools for Migration

**Recommended Scripts**:
```bash
# 1. Find all include statements
grep -r "include.*imtauth" --include="*.h" --include="*.cpp"

# 2. Dependency graph generator
cmake --graphviz=deps.dot .
dot -Tpng deps.dot -o deps.png

# 3. Automated migration script
./migrate-library.sh imtauth auth/core

# 4. Validation
./validate-includes.sh
```

**Git Migration**:
```bash
# Preserve history
git mv Include/imtauth Include/auth/core
git mv Include/imtauthdb Include/auth/db
git mv Include/imtauthgql Include/auth/gql
git mv Include/imtauthgui Include/auth/gui
```
