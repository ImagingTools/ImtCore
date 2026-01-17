# ImtCore Libraries - Include Directory

This directory contains the header and implementation files for all ImtCore libraries. Each subdirectory represents a distinct library with specific functionality.

## 📚 Library Index

### Core Infrastructure (7 libraries)
Foundation libraries that provide base functionality used throughout the system.

- **imtbase** - Core framework with collections, data structures, component management, and plugin system
- **imtcore** - Version information and top-level exports
- **imtcom** - Communication protocols and utilities
- **imtcol** - Collection management, filtering, and header providers
- **imttest** - Testing utilities and helpers
- **imtfile** - File I/O operations and management
- **imtzip** - Compression and archive handling

### Authentication & Authorization (4 libraries)
Complete authentication and authorization system with multiple interfaces.

- **imtauth** - Core authentication/authorization logic (users, roles, sessions, permissions)
- **imtauthdb** - Authentication database persistence layer (SQLite, PostgreSQL)
- **imtauthgql** - Authentication GraphQL API endpoints
- **imtauthgui** - Authentication UI components (login, user management, role editors)

### Database & Persistence (5 libraries)
SQL and NoSQL database connectivity and management.

- **imtdb** - Core SQL database functionality (PostgreSQL, SQLite, migrations, transactions)
- **imtdbgql** - Database GraphQL API layer
- **imtdbgui** - Database management UI components
- **imtmdbx** - MDBX embedded database wrapper
- **imtmongo** - MongoDB integration and utilities

### Licensing System (4 libraries)
Software licensing and activation management.

- **imtlic** - License management core (features, instances, validation)
- **imtlicdb** - License database persistence
- **imtlicgql** - License GraphQL API
- **imtlicgui** - License management UI

### GraphQL APIs (8 libraries)
GraphQL-based API layer for client-server communication.

- **imtgql** - Core GraphQL request/response handling
- **imtclientgql** - Client-side GraphQL operations and caching
- **imtservergql** - Server-side GraphQL schema and resolvers
- **imtguigql** - UI integration with GraphQL
- **imtgqltest** - GraphQL testing framework
- *(See also domain-specific gql libraries: auth, db, lic)*

### User Interface (11 libraries)
Qt/QML-based user interface components and frameworks.

#### Qt Widgets
- **imtgui** - Core Qt GUI widgets and layouts
- **imtwidgets** - Extended widget library
- **imtstyle** - Theme and style management

#### QML/JavaScript
- **imtqml** - Core QML utilities and components
- **imtqml2d** - 2D graphics and drawing in QML
- **imtstylecontrolsqml** - QML style controls and themes

#### Design System
- **imtdesign** - Design tokens and system components

#### Domain-Specific UI
- **imtauthgui** - Authentication UI
- **imtdbgui** - Database management UI
- **imtlicgui** - License management UI
- **imtdevgui** - Developer tools UI
- **imtloggui** - Logging and monitoring UI
- **imtreportgui** - Report viewing and creation UI

### 3D & Visualization (6 libraries)
2D/3D rendering, visualization, and hypermedia content.

- **imt3d** - 3D engine core (shapes, transformations, scene graph)
- **imt3dgui** - 3D GUI components and shapes
- **imt3dview** - 3D viewport and camera controls
- **imthype** - Hypermedia content rendering
- **imthypegui** - Hypermedia UI components
- **imtgeo** - Geographic/geospatial data handling

### Content & Media (5 libraries)
Document, image, and content management.

- **imtimg** - Image processing and manipulation
- **imtdoc** - Document handling and management
- **imtrepo** - Repository and version control
- **imtfile** - File operations (see Core)
- **imtzip** - Compression (see Core)

### Communication & Integration (5 libraries)
Network communication, email, and web services.

- **imthttp** - HTTP client and server implementation
- **imtrest** - REST API framework
- **imtmail** - SMTP email client and message handling
- **imtservice** - Service and daemon framework
- **imtcom** - Core communication (see Core)

### Security (1 library)
Cryptography and encryption services.

- **imtcrypt** - Encryption services (AES, RSA, MD5, SSL/TLS)

### Code Generation & SDL (5 libraries)
Schema-driven development and code generation tools.

- **imtsdl** - Schema Definition Language (SDL) core
- **imtsdlgencpp** - SDL to C++ code generator
- **imtsdlgenqml** - SDL to QML code generator
- **imtddl** - Data Definition Language utilities
- **imtoas** - OpenAPI Specification (OAS) schema support

### Development Tools (4 libraries)
Developer utilities and integrations.

- **imtdev** - Development environment and plugin system
- **imtdevgui** - Developer tools UI
- **imtpy** - Python integration and bindings
- **imtej** - Export and JSON format utilities

### Reporting & Logging (4 libraries)
Logging, monitoring, and report generation.

- **imtlog** - Logging framework and utilities
- **imtloggui** - Logging UI and viewers
- **imtreport** - Reporting engine and templates
- **imtreportgui** - Report viewing and creation UI

### Application Frameworks (3 libraries)
High-level application scaffolding.

- **imtapp** - Desktop application framework and settings
- **imtserverapp** - Server application framework
- **imtupdate** - Software update management

## 🏗️ Architecture Overview

### Layered Architecture

The libraries follow a layered architecture pattern:

```
┌─────────────────────────────────────────────┐
│  Application Layer                          │
│  imtapp, imtserverapp                       │
└─────────────────────────────────────────────┘
                    ↓
┌─────────────────────────────────────────────┐
│  UI Layer (Presentation)                    │
│  *gui, *qml libraries                       │
└─────────────────────────────────────────────┘
                    ↓
┌─────────────────────────────────────────────┐
│  API Layer                                  │
│  *gql, *rest, imthttp                       │
└─────────────────────────────────────────────┘
                    ↓
┌─────────────────────────────────────────────┐
│  Business Logic Layer                       │
│  imtauth, imtlic, domain libraries          │
└─────────────────────────────────────────────┘
                    ↓
┌─────────────────────────────────────────────┐
│  Data Access Layer                          │
│  *db libraries, imtdb, imtmongo             │
└─────────────────────────────────────────────┘
                    ↓
┌─────────────────────────────────────────────┐
│  Core Infrastructure                        │
│  imtbase, imtcore, imtcom, imtcol           │
└─────────────────────────────────────────────┘
```

### Naming Conventions

**Library Name Suffixes:**
- No suffix = Core domain logic
- `gui` = Qt Widget-based user interface
- `qml` = QML/JavaScript interface  
- `gql` = GraphQL API endpoints
- `db` = Database persistence layer
- `gen*` = Code generators

**Examples:**
- `imtauth` - Auth core logic
- `imtauthdb` - Auth database persistence
- `imtauthgql` - Auth GraphQL API
- `imtauthgui` - Auth Qt user interface

## 🔍 Finding the Right Library

### By Functionality

**Need to...**
- **Authenticate users?** → imtauth, imtauthgql, imtauthgui
- **Store data?** → imtdb, imtmdbx, imtmongo + domain-specific *db
- **Build UI?** → imtgui, imtqml, imtwidgets
- **Create API?** → imtgql, imtrest, imthttp
- **Process images?** → imtimg
- **Handle files?** → imtfile, imtzip
- **Generate code?** → imtsdl, imtsdlgencpp, imtsdlgenqml
- **Send emails?** → imtmail
- **Encrypt data?** → imtcrypt
- **Manage licenses?** → imtlic, imtlicgql, imtlicgui
- **Create reports?** → imtreport, imtreportgui
- **Log events?** → imtlog, imtloggui
- **Work with 3D?** → imt3d, imt3dgui, imt3dview

### By Technology

**Working with:**
- **Qt Widgets** → imtgui, imtwidgets, *gui libraries
- **QML** → imtqml, imtqml2d, imtstylecontrolsqml
- **GraphQL** → imtgql, imtclientgql, imtservergql, *gql libraries
- **REST APIs** → imtrest, imthttp
- **SQL Databases** → imtdb, *db libraries
- **MongoDB** → imtmongo
- **Python** → imtpy
- **3D Graphics** → imt3d, imt3dgui, imt3dview

## 📦 Library Dependencies

### Core Libraries (No external dependencies)
These libraries are at the foundation and can be used independently:
- imtcore
- imtbase
- imtcom
- imttest

### Common Dependency Patterns

Most domain libraries follow this pattern:
```
domain-core (e.g., imtauth)
    ↓
domain-db (e.g., imtauthdb) ← depends on imtdb
    ↓
domain-gql (e.g., imtauthgql) ← depends on imtgql
    ↓
domain-gui (e.g., imtauthgui) ← depends on imtgui
```

## 🚀 Getting Started

### For New Developers

1. **Start with the fundamentals:**
   - Read imtbase documentation for component architecture
   - Understand imtgql for API patterns
   - Review imtgui for UI patterns

2. **Explore a complete domain:**
   - Authentication (imtauth → imtauthdb → imtauthgql → imtauthgui)
   - Licensing (imtlic → imtlicdb → imtlicgql → imtlicgui)

3. **Build something:**
   - Use imtapp for desktop applications
   - Use imtserverapp for server applications

### For Contributors

1. **Naming conventions:** Follow existing patterns (core, db, gql, gui)
2. **Dependencies:** Keep layers separated, avoid circular dependencies
3. **Documentation:** Each library should have clear purpose and examples
4. **Testing:** Use imttest utilities for unit tests

## 📖 Additional Resources

- **Full Reorganization Proposal:** See `Docs/LibraryReorganizationProposal.md` for detailed analysis and future improvements
- **Build System:** See `Build/` and `CMake/` directories
- **Examples:** See `Docs/Tutorials/` for usage examples
- **API Documentation:** Generate with Doxygen using `Docs/Doxyfile`

## 🔮 Future Plans

See `Docs/LibraryReorganizationProposal.md` for:
- Proposed hierarchical organization by domain
- Improved naming conventions
- Migration strategy to better structure
- Rationale for grouping decisions

## 📝 Library Count

**Total Libraries:** 61

**By Category:**
- Core Infrastructure: 7
- Authentication: 4
- Database: 5
- Licensing: 4
- GraphQL APIs: 8
- User Interface: 11
- Visualization: 6
- Content/Media: 5
- Communication: 5
- Security: 1
- Code Generation: 5
- Development Tools: 4
- Reporting/Logging: 4
- Application Frameworks: 3

## ⚠️ Current Limitations

The current flat structure has some challenges:
- All 61 libraries in one directory makes navigation difficult
- Related libraries (e.g., auth suite) are not visually grouped
- No clear separation between infrastructure and application code
- Naming alone must convey purpose and relationships

These are addressed in the reorganization proposal. For now, use this index as your guide.

---

**Last Updated:** January 2026  
**Maintainer:** ImtCore Development Team
