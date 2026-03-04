// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


/**
\defgroup ImtCore ImagingTools Core SDK
\mainpage ImtCore - ImagingTools Core SDK
\section Introduction
ImagingTools Core SDK is an extension framework for ACF (Application Component Framework).
The main goal of this framework is the implementation of components and interfaces for realizing business logic
that can find general application in every software product.

\section LibraryOrganization Library Organization
The ImtCore framework consists of 61 libraries organized in the Include/ directory, grouped into 13 technical domains.
For a complete overview of all libraries, their purposes, and relationships, see:
- Include/README.md - Complete library index with categorization
- Docs/LibraryQuickReference.md - Quick lookup guide for finding the right library
- Docs/LibraryArchitectureDiagrams.md - Visual diagrams of library structure
- Docs/LibraryReorganizationProposal.md - Detailed proposal for future improvements

Libraries are organized by suffix to indicate technical layers:
- No suffix: Core business logic (e.g., imtauth)
- *gui: Qt-based user interface components (e.g., imtauthgui)
- *gql: GraphQL API endpoints (e.g., imtauthgql)
- *db: Database persistence layers (e.g., imtauthdb)
- *qml: QML/JavaScript UI components (e.g., imtqml)

\section TechnicalDomains Technical Domains

The ImtCore SDK is organized into the following technical domains:

\subsection CoreInfrastructure 1. Core Infrastructure (7 libraries)
Foundation libraries providing base functionality used throughout the system.
- imtbase - Core framework with collections, data structures, component management, plugin system
- imtcore - Version information and top-level exports
- imtcom - Server communication and connection management (\ref Include/imtcom/README.md)
- imtcol - Collection management, filtering, and header providers
- imttest - Testing utilities and helpers
- imtfile - File I/O operations and management
- imtzip - Compression and archive handling

\subsection AuthenticationAuthorization 2. Authentication & Authorization (4 libraries)
Complete authentication and authorization system with multiple persistence backends.
- imtauth - Core authentication/authorization logic (users, roles, sessions, permissions)
- imtauthdb - Authentication database persistence layer (SQLite, PostgreSQL)
- imtauthgql - Authentication GraphQL API endpoints
- imtauthgui - Authentication UI components (login, user management, role editors)

\subsection DatabasePersistence 3. Database & Persistence (5 libraries)
SQL and NoSQL database connectivity and management.
- imtdb - Core SQL database functionality (PostgreSQL, SQLite, migrations, transactions)
- imtdbgql - Database GraphQL API layer
- imtdbgui - Database management UI components
- imtmdbx - MDBX embedded database wrapper
- imtmongo - MongoDB integration and utilities

\subsection LicensingSystem 4. Licensing System (4 libraries)
Software and hardware product license management. (\ref Include/imtlic/README.md, \ref Include/imtlicgql/README.md, \ref Include/imtlicdb/README.md)
- imtlic - License management core (features, instances, validation)
- imtlicdb - License database persistence
- imtlicgql - License GraphQL API
- imtlicgui - License management UI

\subsection GraphQLAPIs 5. GraphQL APIs (8 libraries)
GraphQL-based API layer for client-server communication.
- imtgql - Core GraphQL request/response handling
- imtclientgql - Client-side GraphQL operations and caching
- imtservergql - Server-side GraphQL schema and resolvers (\ref Include/imtservergql/README.md)
- imtguigql - UI integration with GraphQL
- imtgqltest - GraphQL testing framework
- Plus domain-specific GraphQL APIs: imtauthgql, imtlicgql, imtdbgql

\subsection UserInterface 6. User Interface (16 libraries)
Qt/QML-based user interface components and frameworks.

Core UI Libraries (7):
- imtgui - Core Qt GUI widgets and layouts
- imtwidgets - Extended widget library
- imtstyle - Theme and style management
- imtqml - Core QML utilities and components
- imtqml2d - 2D graphics and drawing in QML
- imtstylecontrolsqml - QML style controls and themes
- imtdesign - Design tokens and system components

Domain-Specific GUI Libraries (9):
- imt3dgui, imthypegui, imtauthgui, imtdbgui, imtlicgui, imtdevgui, imtloggui, imtreportgui

\subsection Visualization3D 7. 3D & Visualization (6 libraries)
2D/3D rendering, visualization, and hypermedia content.
- imt3d - 3D engine core (shapes, transformations, scene graph)
- imt3dgui - 3D GUI components and shapes
- imt3dview - 3D viewport and camera controls
- imthype - Hypermedia content rendering
- imthypegui - Hypermedia UI components
- imtgeo - Geographic/geospatial data handling

\subsection ContentMedia 8. Content & Media (3 libraries)
Document, image, and content management.
- imtimg - Image processing and manipulation
- imtdoc - Document handling and management
- imtrepo - Repository and version control

\subsection CommunicationIntegration 9. Communication & Integration (4 libraries)
Network communication, email, and web services.
- imthttp - HTTP client and server implementation
- imtrest - REST API framework
- imtmail - SMTP email client and message handling
- imtservice - Service and daemon framework

\subsection Security 10. Security (1 library)
Cryptography and encryption services.
- imtcrypt - Encryption services (AES, RSA, MD5, SSL/TLS)

\subsection CodeGenerationSDL 11. Code Generation & SDL (5 libraries)
Schema-driven development and code generation tools. See Docs/SDL_Documentation_Index.md for SDL documentation.
- imtsdl - Schema Definition Language (SDL) core
- imtsdlgencpp - SDL to C++ code generator
- imtsdlgenqml - SDL to QML code generator
- imtddl - Data Definition Language utilities
- imtoas - OpenAPI Specification (OAS) schema support

\subsection DevelopmentTools 12. Development Tools (4 libraries)
Developer utilities and integrations.
- imtdev - Development environment and plugin system
- imtdevgui - Developer tools UI
- imtpy - Python integration and bindings
- imtej - Export and JSON format utilities

\subsection ReportingLogging 13. Reporting & Logging (4 libraries)
Logging, monitoring, and report generation.
- imtlog - Logging framework and utilities
- imtloggui - Logging UI and viewers
- imtreport - Reporting engine and templates
- imtreportgui - Report viewing and creation UI

\subsection ApplicationFrameworks 14. Application Frameworks (3 libraries)
High-level application scaffolding.
- imtapp - Desktop application framework and settings
- imtserverapp - Server application framework
- imtupdate - Software update management

\defgroup Collection Collection
\section Motivation Motivation
One of the most important aspects in the software development is the definition and implementation of the data layer.
A general task here is the grouping of data, since we can assume that any software product must manage sets of entities. We call such sets of entities - Collection.
The typical requirements for an entity collection are:
- Iterations over the collection
- Searching for entities based on one or more conditions
- Filtering of entities
- Reading and writing of entity data

The aim of the collection module is to standardize the working with object collections through a set of general interfaces and the associated implementations
\section Introduction Introduction
A collection is a container of items. We do not know anything about the structure or implementation detailes of the items in the container.
The only thing we know about this container is that the items, that are located there, can be clearly identified (e.g. via a UUID).
With regard to a collection, we therefore differentiate between two types of interfaces:
- Accessing to the items inside a collection
- Accessing to the information about the contents of a collection

The basic interface for access to information about a collection is imtbase::ICollectionInfo.
This simple interface allows querying all existing IDs of the elements in the collection.
If the IDs of elements are known, the properties (descriptions) of these elements can be accessed (via these IDs)

For a collection itself, we don't have a general interface.
Instead, there are interfaces for certain types of collections, the most important one is - imtbase::IObjectCollection.
This interface describes the collection of data objects, that is, objects that implement directly or indirectly the istd::IChangeable interface.
"Concrete" collection interfaces can offer their specialized extensions to the imtbase::ICollectionInfo interface.
There is a suitable imtbase::IObjectCollectionInfo interface for imtbase::IObjectCollection.

\section ObjectCollection Object Collection
As already said, for the collection of data objects we provide the interface imtbase::IObjectCollection. This interface allows CRUD/L operations for a document container.
There are following implementations of object/document collections:
- In-Memory collection (imtbase::CObjectCollection, imtbase::CObjectCollectionComp)
- File-based collection (imtrepo::CFileCollectionComp)
- SQL-Database collection (imtdb::CSqlDatabaseObjectCollectionComp)

An important aspect is the management of informations about objects containing in the collection. We differentiate between of followng kinds of such informations:
- Permanent/static information about a object/document in the collection such as Name or Description
- Meta-information related the object in the collection. For example Insertion Time, Last Modification Time. Important: these meta-informations are related to the Collection and not to the data object!
- Meta-information of the data object itself. Each object/document can have its own meta-informations

\defgroup FileRepository File Repository
\section Motivation Motivation
In the document-oriented application you need persistence of the documents or another grouped application data. File-based document repository is a way to do it.
File collections are specializations of imtbase::IObjectCollection interfrace.

\defgroup LicenseManagement License Management
\section Motivation Motivation
In many applications it is desirable that the user only has limited access to the implemented software functions. A licensing mechanism that manages these access rights is often used for this purpose.
\section Concept
1. Definition of license data
2. Provide management of license collections
3. License check using (via binding more general interface iauth::IRightsProvider)
4. Low-level license key generation (Encryption)
5. Device persistence of license data.
6. File-based peristence of license data.
7. Diagnostics information about the license status in the application

We define following types of license data:
1. List of all product variants. This includes the overview of all available products and the associated licenses.
2. List of product installations.
3. Encrypted information file containing all license keys for a given product installation.

Each product application has knowledge about all available product licenses and 
activate them by reading and validating the license keys from the license file containing license keys.
The license keys will be defined by Product Manager before shipping of the installation setup to the customer.

*/
