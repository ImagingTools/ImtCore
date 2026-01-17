# ImtCore Library Architecture Diagrams

Visual representations of the ImtCore library structure and relationships.

## Current Library Organization (Flat Structure)

```
Include/
├── imt3d
├── imt3dgui
├── imt3dview
├── imtapp
├── imtauth
├── imtauthdb
├── imtauthgql
├── imtauthgui
├── imtbase
├── imtclientgql
├── imtcol
├── imtcom
├── imtcore
├── imtcrypt
├── imtdb
├── imtdbgql
├── imtdbgui
├── imtddl
├── imtdesign
├── imtdev
├── imtdevgui
├── imtdoc
├── imtej
├── imtfile
├── imtgeo
├── imtgql
├── imtgqltest
├── imtgui
├── imtguigql
├── imthttp
├── imthype
├── imthypegui
├── imtimg
├── imtlic
├── imtlicdb
├── imtlicgql
├── imtlicgui
├── imtlog
├── imtloggui
├── imtmail
├── imtmdbx
├── imtmongo
├── imtoas
├── imtpy
├── imtqml
├── imtqml2d
├── imtrepo
├── imtreport
├── imtreportgui
├── imtrest
├── imtsdl
├── imtsdlgencpp
├── imtsdlgenqml
├── imtserverapp
├── imtservergql
├── imtservice
├── imtstyle
├── imtstylecontrolsqml
├── imttest
├── imtupdate
├── imtwidgets
└── imtzip

Total: 61 libraries in flat structure
```

## Libraries by Suffix/Category

```
┌─────────────────────────────────────────────────────────────┐
│                    CORE LIBRARIES (no suffix)               │
│  imtbase, imtcore, imtcom, imtcol, imttest, imtfile,       │
│  imtzip, imtcrypt, imtmail, imthttp, imtrest, imtservice   │
│                         (12 libraries)                      │
└─────────────────────────────────────────────────────────────┘
                            ↑ Used by ↑
┌─────────────────────────────────────────────────────────────┐
│               DOMAIN LOGIC LIBRARIES (no suffix)            │
│  imtauth, imtlic, imtdb, imtimg, imtdoc, imtrepo,          │
│  imt3d, imtgeo, imthype, imtreport, imtlog, imtdev,        │
│  imtmdbx, imtmongo, imtgql, imtsdl, imtddl, imtoas,        │
│  imtej, imtpy, imtdesign                                    │
│                         (21 libraries)                      │
└─────────────────────────────────────────────────────────────┘
                            ↑ Used by ↑
┌─────────────────────────────────────────────────────────────┐
│              DATABASE LAYER (*db suffix)                    │
│       imtauthdb, imtlicdb                                   │
│                          (2 libraries)                      │
└─────────────────────────────────────────────────────────────┘
                            ↑ Used by ↑
┌─────────────────────────────────────────────────────────────┐
│              API LAYER (*gql suffix)                        │
│  imtauthgql, imtlicgql, imtdbgql, imtguigql,               │
│  imtclientgql, imtservergql, imtgqltest                    │
│                          (7 libraries)                      │
└─────────────────────────────────────────────────────────────┘
                            ↑ Used by ↑
┌─────────────────────────────────────────────────────────────┐
│              UI LAYER (*gui/*qml suffix)                    │
│  imtauthgui, imtlicgui, imtdbgui, imtdevgui,               │
│  imtloggui, imtreportgui, imt3dgui, imthypegui,            │
│  imtgui, imtwidgets, imtqml, imtqml2d,                     │
│  imtstyle, imtstylecontrolsqml                             │
│                         (14 libraries)                      │
└─────────────────────────────────────────────────────────────┘
                            ↑ Used by ↑
┌─────────────────────────────────────────────────────────────┐
│           APPLICATION LAYER (*app suffix)                   │
│       imtapp, imtserverapp, imtupdate                       │
│                          (3 libraries)                      │
└─────────────────────────────────────────────────────────────┘
                            ↑ Used by ↑
┌─────────────────────────────────────────────────────────────┐
│                  CODE GENERATION (*gen*)                    │
│       imtsdlgencpp, imtsdlgenqml, imt3dview                │
│                          (3 libraries)                      │
└─────────────────────────────────────────────────────────────┘
```

## Authentication Domain Example

Shows how one domain (auth) spans multiple technical layers:

```
┌──────────────────────────────────────────────────────────────┐
│                   AUTHENTICATION DOMAIN                       │
│                                                               │
│  ┌─────────────────────────────────────────────────────┐    │
│  │  imtauthgui - Qt Widgets                            │    │
│  │  • Login dialog                                     │    │
│  │  • User management UI                               │    │
│  │  • Role editors                                     │    │
│  └───────────────────────┬─────────────────────────────┘    │
│                          ↓ uses                              │
│  ┌─────────────────────────────────────────────────────┐    │
│  │  imtauthgql - GraphQL API                           │    │
│  │  • loginMutation                                    │    │
│  │  • userQueries                                      │    │
│  │  • roleQueries                                      │    │
│  └───────────────────────┬─────────────────────────────┘    │
│                          ↓ uses                              │
│  ┌─────────────────────────────────────────────────────┐    │
│  │  imtauthdb - Database Persistence                   │    │
│  │  • User CRUD                                        │    │
│  │  • Role storage                                     │    │
│  │  • Session management                               │    │
│  └───────────────────────┬─────────────────────────────┘    │
│                          ↓ uses                              │
│  ┌─────────────────────────────────────────────────────┐    │
│  │  imtauth - Core Logic                               │    │
│  │  • Authentication logic                             │    │
│  │  • Authorization rules                              │    │
│  │  • Permission system                                │    │
│  │  • Session tokens                                   │    │
│  └─────────────────────────────────────────────────────┘    │
│                                                               │
└──────────────────────────────────────────────────────────────┘
```

## Full System Architecture (Layer View)

```
┌──────────────────────────────────────────────────────────────┐
│ LAYER 7: Applications                                        │
│ ┌────────────────┐  ┌──────────────┐  ┌─────────────────┐  │
│ │    imtapp      │  │ imtserverapp │  │   imtupdate     │  │
│ │   Desktop App  │  │  Server App  │  │  Auto-update    │  │
│ └────────────────┘  └──────────────┘  └─────────────────┘  │
└──────────────────────────────────────────────────────────────┘
                            ↓ depends on ↓
┌──────────────────────────────────────────────────────────────┐
│ LAYER 6: User Interface                                      │
│ ┌─────────┐ ┌──────────┐ ┌─────────┐ ┌──────────────────┐  │
│ │ imtgui  │ │imtwidgets│ │ imtqml  │ │   *gui libs      │  │
│ │Qt Widgets│ │Extended │ │QML Core │ │(auth, lic, db...)│  │
│ └─────────┘ └──────────┘ └─────────┘ └──────────────────┘  │
└──────────────────────────────────────────────────────────────┘
                            ↓ depends on ↓
┌──────────────────────────────────────────────────────────────┐
│ LAYER 5: API                                                 │
│ ┌──────────┐ ┌────────────┐ ┌────────────┐ ┌────────────┐  │
│ │  imtgql  │ │imtclientgql│ │imtservergql│ │  *gql libs │  │
│ │ GraphQL  │ │GQL Client  │ │ GQL Server │ │ (auth,lic) │  │
│ └──────────┘ └────────────┘ └────────────┘ └────────────┘  │
│ ┌──────────┐ ┌──────────┐                                   │
│ │  imtrest │ │ imthttp  │                                   │
│ │   REST   │ │   HTTP   │                                   │
│ └──────────┘ └──────────┘                                   │
└──────────────────────────────────────────────────────────────┘
                            ↓ depends on ↓
┌──────────────────────────────────────────────────────────────┐
│ LAYER 4: Business Logic / Domain                            │
│ ┌─────────┐ ┌─────────┐ ┌─────────┐ ┌──────────────────┐  │
│ │ imtauth │ │ imtlic  │ │ imt3d   │ │   imtreport      │  │
│ │  Auth   │ │ License │ │   3D    │ │    Reports       │  │
│ └─────────┘ └─────────┘ └─────────┘ └──────────────────┘  │
│ ┌─────────┐ ┌─────────┐ ┌─────────┐ ┌──────────────────┐  │
│ │ imtimg  │ │ imtdoc  │ │ imtgeo  │ │    imthype       │  │
│ │ Images  │ │  Docs   │ │   Geo   │ │   Hypermedia     │  │
│ └─────────┘ └─────────┘ └─────────┘ └──────────────────┘  │
└──────────────────────────────────────────────────────────────┘
                            ↓ depends on ↓
┌──────────────────────────────────────────────────────────────┐
│ LAYER 3: Data Access                                         │
│ ┌──────────┐ ┌───────────┐ ┌──────────┐ ┌─────────────┐   │
│ │  imtdb   │ │ imtmongo  │ │ imtmdbx  │ │   *db libs  │   │
│ │   SQL    │ │  MongoDB  │ │   MDBX   │ │ (auth, lic) │   │
│ └──────────┘ └───────────┘ └──────────┘ └─────────────┘   │
└──────────────────────────────────────────────────────────────┘
                            ↓ depends on ↓
┌──────────────────────────────────────────────────────────────┐
│ LAYER 2: Infrastructure Services                            │
│ ┌──────────┐ ┌──────────┐ ┌──────────┐ ┌─────────────┐   │
│ │ imtcrypt │ │ imtfile  │ │ imtzip   │ │  imtmail    │   │
│ │ Encrypt  │ │   File   │ │Compress  │ │   Email     │   │
│ └──────────┘ └──────────┘ └──────────┘ └─────────────┘   │
│ ┌──────────┐ ┌──────────┐ ┌──────────┐                    │
│ │  imtlog  │ │imtservice│ │ imtcom   │                    │
│ │   Log    │ │ Services │ │   Comm   │                    │
│ └──────────┘ └──────────┘ └──────────┘                    │
└──────────────────────────────────────────────────────────────┘
                            ↓ depends on ↓
┌──────────────────────────────────────────────────────────────┐
│ LAYER 1: Core Foundation                                     │
│ ┌──────────┐ ┌──────────┐ ┌──────────┐ ┌─────────────┐   │
│ │ imtbase  │ │ imtcore  │ │ imtcol   │ │  imttest    │   │
│ │Framework │ │ Version  │ │Collection│ │   Testing   │   │
│ └──────────┘ └──────────┘ └──────────┘ └─────────────┘   │
└──────────────────────────────────────────────────────────────┘
```

## Code Generation Flow

```
┌──────────────────────────────────────────────────────────────┐
│                      SDL Schema Files                        │
│                         (.sdl)                               │
└────────────────────┬─────────────────────────────────────────┘
                     ↓ parsed by
              ┌──────────────┐
              │   imtsdl     │
              │  SDL Parser  │
              └──────┬───────┘
                     ↓ generates
         ┌───────────┴───────────┐
         ↓                       ↓
  ┌──────────────┐        ┌──────────────┐
  │imtsdlgencpp  │        │imtsdlgenqml  │
  │C++ Generator │        │QML Generator │
  └──────┬───────┘        └──────┬───────┘
         ↓                       ↓
  ┌──────────────┐        ┌──────────────┐
  │   .cpp/.h    │        │    .qml      │
  │   files      │        │    files     │
  └──────────────┘        └──────────────┘
         ↓                       ↓
  ┌──────────────┐        ┌──────────────┐
  │   Business   │        │   UI Code    │
  │    Logic     │        │              │
  └──────────────┘        └──────────────┘
```

## GraphQL Architecture

```
┌──────────────────────────────────────────────────────────────┐
│                        Client Side                           │
│  ┌──────────────────────────────────────────────────────┐   │
│  │              imtclientgql                            │   │
│  │  • Query execution                                   │   │
│  │  • Mutation handling                                 │   │
│  │  • Subscription management                           │   │
│  │  • Caching                                           │   │
│  └────────────────────┬─────────────────────────────────┘   │
└───────────────────────┼──────────────────────────────────────┘
                        ↓ HTTP/WebSocket
┌───────────────────────┼──────────────────────────────────────┐
│                       ↓ Server Side                          │
│  ┌──────────────────────────────────────────────────────┐   │
│  │              imtservergql                            │   │
│  │  • Schema definition                                 │   │
│  │  • Resolvers                                         │   │
│  │  • Subscriptions                                     │   │
│  └────────────────────┬─────────────────────────────────┘   │
│                       ↓ delegates to                         │
│  ┌────────────────────────────────────────────────────┐     │
│  │         Domain-specific GQL libraries              │     │
│  │  ┌──────────────┐ ┌──────────────┐ ┌────────────┐ │     │
│  │  │  imtauthgql  │ │  imtlicgql   │ │ imtdbgql   │ │     │
│  │  │  Auth API    │ │  License API │ │  DB API    │ │     │
│  │  └──────┬───────┘ └──────┬───────┘ └─────┬──────┘ │     │
│  └─────────┼─────────────────┼───────────────┼────────┘     │
│            ↓                 ↓               ↓              │
│  ┌────────────────────────────────────────────────────┐     │
│  │         Domain Logic & Database                    │     │
│  │  ┌──────────────┐ ┌──────────────┐ ┌────────────┐ │     │
│  │  │   imtauth    │ │    imtlic    │ │   imtdb    │ │     │
│  │  │ └─────┬──────┘ │ └─────┬──────┘ │ └────┬─────┘ │     │
│  │  │   imtauthdb  │ │   imtlicdb   │ │      │       │     │
│  │  └──────────────┘ └──────────────┘ └──────────────┘     │
│  └────────────────────────────────────────────────────┘     │
└──────────────────────────────────────────────────────────────┘
```

## Proposed Reorganization (Domain-First)

```
Include/
│
├── core/                    # Foundation
│   ├── base/
│   ├── core/
│   ├── collections/
│   └── testing/
│
├── auth/                    # Authentication domain
│   ├── core/
│   ├── db/
│   ├── gql/
│   └── gui/
│
├── licensing/               # Licensing domain
│   ├── core/
│   ├── db/
│   ├── gql/
│   └── gui/
│
├── database/                # Data infrastructure
│   ├── core/
│   ├── gql/
│   ├── gui/
│   ├── mdbx/
│   └── mongodb/
│
├── visualization/           # Graphics & rendering
│   ├── 3d/
│   │   ├── core/
│   │   ├── gui/
│   │   └── view/
│   ├── hypermedia/
│   └── geospatial/
│
├── api/                     # API layer
│   ├── graphql/
│   │   ├── core/
│   │   ├── client/
│   │   ├── server/
│   │   ├── gui/
│   │   └── testing/
│   ├── rest/
│   └── http/
│
├── ui/                      # User interface
│   ├── core/
│   ├── widgets/
│   ├── qml/
│   └── styling/
│
├── content/                 # Content management
│   ├── files/
│   ├── images/
│   ├── documents/
│   └── compression/
│
├── security/                # Security services
│   └── cryptography/
│
├── communication/           # External comm
│   ├── email/
│   └── service/
│
├── reporting/               # Reporting & logging
│   ├── reports/
│   └── logging/
│
├── codegen/                 # Code generation
│   ├── sdl/
│   └── ddl/
│
├── development/             # Dev tools
│   ├── core/
│   ├── gui/
│   └── python/
│
└── application/             # App frameworks
    ├── desktop/
    ├── server/
    └── updates/
```

## Domain Relationships

```
┌─────────────┐        ┌─────────────┐        ┌─────────────┐
│    auth/    │◄──────►│   licensing/ │◄──────►│  database/  │
│             │        │              │        │             │
│ • Users     │        │ • Features   │        │ • SQL       │
│ • Roles     │        │ • Activation │        │ • NoSQL     │
│ • Sessions  │        │ • Validation │        │ • Migration │
└──────┬──────┘        └──────┬───────┘        └──────┬──────┘
       │                      │                       │
       └──────────────┬───────┴───────────────────────┘
                      ↓
              ┌───────────────┐
              │     api/      │
              │               │
              │ • GraphQL     │
              │ • REST        │
              │ • HTTP        │
              └───────┬───────┘
                      ↓
              ┌───────────────┐
              │      ui/      │
              │               │
              │ • Qt Widgets  │
              │ • QML         │
              │ • Styling     │
              └───────────────┘
```

## Benefits of Reorganization

### Current (Flat):
```
Finding auth code:
  → Search 61 libraries
  → Find: imtauth, imtauthdb, imtauthgql, imtauthgui
  → Time: ~2-3 minutes
```

### Proposed (Hierarchical):
```
Finding auth code:
  → Navigate to auth/
  → See: core/, db/, gql/, gui/
  → Time: ~10 seconds
```

---

**For complete details, see:**
- `Docs/LibraryReorganizationProposal.md` - Full proposal
- `Include/README.md` - Library index
- `Docs/LibraryQuickReference.md` - Quick lookup guide
