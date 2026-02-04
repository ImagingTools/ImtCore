# ImtCore Library Quick Reference

A condensed reference for quickly finding the right library for your task.

## Quick Lookup by Purpose

| What do you need? | Use this library |
|-------------------|------------------|
| Base framework, collections, components | `imtbase` |
| User login, permissions, roles | `imtauth` + `imtauthgql` + `imtauthgui` |
| SQL database (PostgreSQL, SQLite) | `imtdb` |
| NoSQL database (MongoDB) | `imtmongo` |
| Embedded database (MDBX) | `imtmdbx` |
| License management, activation | `imtlic` + `imtlicgql` + `imtlicgui` |
| GraphQL API client | `imtclientgql` |
| GraphQL API server | `imtservergql` |
| HTTP client/server | `imthttp` |
| REST API | `imtrest` |
| Qt GUI widgets | `imtgui` + `imtwidgets` |
| QML components | `imtqml` + `imtqml2d` |
| 3D rendering | `imt3d` + `imt3dgui` + `imt3dview` |
| Image processing | `imtimg` |
| File operations | `imtfile` |
| Compression/ZIP | `imtzip` |
| Encryption (AES, RSA) | `imtcrypt` |
| Email/SMTP | `imtmail` |
| Logging | `imtlog` + `imtloggui` |
| Reports | `imtreport` + `imtreportgui` |
| Code generation from SDL | `imtsdl` + `imtsdlgencpp` + `imtsdlgenqml` |
| Desktop application | `imtapp` |
| Server application | `imtserverapp` |
| Software updates | `imtupdate` |
| Testing utilities | `imttest` |
| Python integration | `imtpy` |
| Design tokens | `imtdesign` |

## Library Suffixes Explained

| Suffix | Meaning | Example |
|--------|---------|---------|
| *(none)* | Core business logic | `imtauth` - authentication core |
| `gui` | Qt Widget UI components | `imtauthgui` - auth UI dialogs |
| `gql` | GraphQL API endpoints | `imtauthgql` - auth API |
| `db` | Database persistence | `imtauthdb` - auth data layer |
| `qml` | QML/JavaScript UI | `imtqml` - QML utilities |
| `gen*` | Code generators | `imtsdlgencpp` - C++ generator |
| `app` | Application framework | `imtapp` - desktop app |

## Common Library Combinations

### Building an Authenticated Web App
```
imtbase              # Base framework
↓
imtauth              # Authentication logic
↓
imtauthdb            # User storage
↓
imtauthgql           # Auth API
↓
imtservergql         # GraphQL server
↓
imtclientgql         # Client connection
↓
imtauthgui           # Login UI
```

### Creating a Desktop Application
```
imtbase              # Base framework
↓
imtgui               # GUI framework
↓
imtwidgets           # Widget library
↓
imtapp               # Application framework
```

### Building a Data-Driven Feature
```
imtbase              # Base framework
↓
imtdb                # Database core
↓
imtgql               # GraphQL core
↓
[your-feature]       # Your domain logic
↓
[your-feature]gql    # Your API
↓
[your-feature]gui    # Your UI
```

## Dependency Layers (Bottom to Top)

```
Layer 7: Applications
         imtapp, imtserverapp
         ↑
Layer 6: UI Components
         imtgui, imtqml, *gui libraries
         ↑
Layer 5: API Endpoints
         *gql, imtrest, imthttp
         ↑
Layer 4: Business Logic
         imtauth, imtlic, domain libs
         ↑
Layer 3: Data Access
         *db, imtdb, imtmongo
         ↑
Layer 2: Infrastructure
         imtcrypt, imtfile, imtmail
         ↑
Layer 1: Foundation
         imtbase, imtcore, imtcom
```

## Domain-Based Library Groups

### Authentication Domain
- `imtauth` - Core authentication/authorization
- `imtauthdb` - Auth database layer
- `imtauthgql` - Auth GraphQL API
- `imtauthgui` - Auth user interface

### Licensing Domain
- `imtlic` - License management
- `imtlicdb` - License database
- `imtlicgql` - License API
- `imtlicgui` - License UI

### Database Domain
- `imtdb` - SQL database core
- `imtdbgql` - Database API
- `imtdbgui` - Database UI
- `imtmdbx` - MDBX embedded DB
- `imtmongo` - MongoDB integration

### GraphQL Domain
- `imtgql` - GraphQL core
- `imtclientgql` - Client operations
- `imtservergql` - Server schema
- `imtguigql` - UI integration
- `imtgqltest` - Testing tools

### 3D Visualization Domain
- `imt3d` - 3D engine core
- `imt3dgui` - 3D GUI components
- `imt3dview` - 3D viewport

### UI Domain
- `imtgui` - Core GUI widgets
- `imtwidgets` - Extended widgets
- `imtqml` - QML utilities
- `imtqml2d` - 2D QML graphics
- `imtstyle` - Styling/themes
- `imtstylecontrolsqml` - QML controls
- `imtdesign` - Design system

### Content Domain
- `imtimg` - Image processing
- `imtdoc` - Document handling
- `imtfile` - File operations
- `imtzip` - Compression
- `imtrepo` - Repository management

### Communication Domain
- `imthttp` - HTTP protocol
- `imtrest` - REST APIs
- `imtmail` - Email/SMTP
- `imtcom` - Core communication
- `imtservice` - Service framework

### Code Generation Domain
- `imtsdl` - Schema definition
- `imtsdlgencpp` - C++ generator
- `imtsdlgenqml` - QML generator
- `imtddl` - Data definition
- `imtoas` - OpenAPI schemas

### Reporting Domain
- `imtreport` - Report engine
- `imtreportgui` - Report UI
- `imtlog` - Logging framework
- `imtloggui` - Logging UI

### Development Domain
- `imtdev` - Dev tools core
- `imtdevgui` - Dev tools UI
- `imtpy` - Python bindings
- `imtej` - Export/JSON utils
- `imttest` - Testing utilities

## Most Commonly Used Libraries

🔥 **Core Dependencies** (used by almost everything)
- `imtbase` - Base framework
- `imtcore` - Version info

⭐ **Popular Libraries** (used by many features)
- `imtdb` - Database operations
- `imtgql` - GraphQL APIs
- `imtgui` - User interfaces
- `imtauth` - Authentication

💎 **Specialized But Important**
- `imtcrypt` - Security/encryption
- `imtfile` - File handling
- `imtlog` - Logging/diagnostics
- `imttest` - Testing

## When to Create a New Library

Create a new library when:
1. ✅ Functionality is reusable across multiple projects
2. ✅ Code has minimal dependencies on other features
3. ✅ Library can be tested independently
4. ✅ Clear, focused purpose

Don't create a new library when:
1. ❌ Code is specific to one feature
2. ❌ Tightly coupled to existing library
3. ❌ Less than ~500 lines of code
4. ❌ Would increase complexity more than it helps

## Naming Your Library

Follow these conventions:
```
[imt][domain][suffix]

imt      - Always present (ImagingTools prefix)
domain   - Short name (3-10 chars): auth, lic, img, doc
suffix   - Optional technical layer:
           gui (Qt), gql (GraphQL), db (database),
           qml (QML/JS), gen* (generator)
```

**Good Examples:**
- `imtauth` - Authentication domain, core
- `imtauthgql` - Authentication GraphQL API
- `imtimg` - Image processing
- `imtsdlgencpp` - SDL to C++ generator

**Avoid:**
- `imtauthenticationmanager` - Too long
- `imtutil` - Too vague
- `auth` - Missing imt prefix
- `imtgqlauth` - Wrong suffix order

## Include Headers

```cpp
// Core framework
#include <imtbase/...>
#include <imtcore/Version.h>

// Domain-specific
#include <imtauth/...>
#include <imtauthgql/...>
#include <imtauthgui/...>

// UI
#include <imtgui/...>
#include <imtqml/...>

// Infrastructure
#include <imtdb/...>
#include <imtcrypt/...>
```

## CMake Integration

```cmake
# Link to libraries
target_link_libraries(YourTarget
    imtbase
    imtauth
    imtauthgql
    imtgui
)

# Include directories (usually automatic)
target_include_directories(YourTarget PRIVATE
    ${IMTCORE_DIR}/Include
)
```

## Need More Details?

- **Complete Index:** See `Include/README.md`
- **Reorganization Proposal:** See `Docs/LibraryReorganizationProposal.md`
- **API Docs:** Generate with `doxygen Docs/Doxyfile`
- **Examples:** Check `Docs/Tutorials/`

---

💡 **Tip:** Bookmark this file for quick reference during development!
