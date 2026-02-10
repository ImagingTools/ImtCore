# Licensing System Architecture - ImtCore

## Table of Contents

1. [Overview](#overview)
2. [Module Structure](#module-structure)
3. [Architectural Layers](#architectural-layers)
4. [Module Details](#module-details)
   - [imtlic - Licensing Core](#imtlic---licensing-core)
   - [imtlicgql - GraphQL Interface](#imtlicgql---graphql-interface)
   - [imtlicdb - Database Layer](#imtlicdb---database-layer)
5. [Design Patterns](#design-patterns)
6. [Data Flow](#data-flow)
7. [Key Concepts](#key-concepts)
8. [Integration Points](#integration-points)

---

## Overview

The ImtCore licensing system is a comprehensive, three-layer architecture for managing software and hardware product licenses. The system implements a flexible, feature-based licensing model that separates product definitions from deployed instances and license types from activated licenses.

**Core Philosophy:**
- Products are containers of Features
- License Definitions specify which Features they unlock
- Product Instances deploy products to customers
- License Instances activate licenses with temporal control (expiration dates)

---

## Module Structure

```
┌─────────────────────────────────────────────────────────────┐
│                    Client Applications                       │
│              (Web UI, Desktop Apps, Mobile)                  │
└────────────────────────┬────────────────────────────────────┘
                         │ GraphQL API
                         ▼
┌─────────────────────────────────────────────────────────────┐
│                   imtlicgql Module                           │
│          GraphQL Interface & Business Logic Layer            │
│  ┌─────────────┐  ┌──────────────┐  ┌──────────────┐       │
│  │ Collection  │  │  Controllers │  │  Providers   │       │
│  │ Controllers │  │              │  │              │       │
│  └─────────────┘  └──────────────┘  └──────────────┘       │
└────────────────────────┬────────────────────────────────────┘
                         │ Domain Model
                         ▼
┌─────────────────────────────────────────────────────────────┐
│                     imtlic Module                            │
│              Core Licensing Domain Model                     │
│  ┌──────────┐  ┌──────────┐  ┌─────────────┐               │
│  │ Products │  │ Features │  │  Licenses   │               │
│  │ Instances│  │ Info     │  │ Definitions │               │
│  └──────────┘  └──────────┘  └─────────────┘               │
└────────────────────────┬────────────────────────────────────┘
                         │ Data Access
                         ▼
┌─────────────────────────────────────────────────────────────┐
│                   imtlicdb Module                            │
│              Database Persistence Layer                      │
│  ┌──────────────┐  ┌─────────────┐  ┌──────────────┐       │
│  │   License    │  │   Product   │  │   Feature    │       │
│  │   Delegate   │  │   Delegate  │  │   Delegate   │       │
│  └──────────────┘  └─────────────┘  └──────────────┘       │
└────────────────────────┬────────────────────────────────────┘
                         │ SQL/JSON
                         ▼
┌─────────────────────────────────────────────────────────────┐
│               PostgreSQL Database (imtdb)                    │
│         JSON Document Storage with SQL Queries               │
└─────────────────────────────────────────────────────────────┘
```

---

## Architectural Layers

### Layer 1: Core Domain Model (imtlic)

**Responsibility:** Define licensing concepts, business rules, and domain model

**Key Components:**
- Domain entities (Product, Feature, License)
- Business logic (licensing rules, feature dependencies)
- Interface definitions (Provider interfaces)
- Validation and constraint enforcement

**Technology:** Pure C++, Qt Framework, ACF (Application Component Framework)

---

### Layer 2: GraphQL API Interface (imtlicgql)

**Responsibility:** Expose licensing functionality via GraphQL API

**Key Components:**
- Collection Controllers (CRUD operations on collections)
- Object Controllers (Single object operations)
- Data Providers (Query handlers)
- Metadata Delegates (JSON serialization)
- Permission providers (Feature-based access control)

**Technology:** GraphQL schema (SDL), C++ implementation, imtservergql framework

---

### Layer 3: Database Persistence (imtlicdb)

**Responsibility:** Persist and retrieve licensing data from database

**Key Components:**
- Database Delegates (SQL query generation)
- Object-Relational Mapping (JSON document to C++ objects)
- Filtering and search capabilities
- Metadata extraction from JSON documents

**Technology:** PostgreSQL with JSON support, SQL queries, imtdb framework

---

## Module Details

### imtlic - Licensing Core

#### Purpose
Core licensing domain model implementing feature-based licensing with product-instance separation.

#### Key Abstractions

**1. Products (IProductInfo, CProductInfo)**
- Definition of software/hardware offerings
- Container of Features
- Part of product families/categories
- Template for Product Instances

**2. Features (IFeatureInfo, CFeatureInfo)**
- Atomic units of functionality
- Hierarchical organization (parent-child)
- Optional vs. Mandatory flags
- Permission-based access control
- Dependency management

**3. License Definitions (ILicenseDefinition, CLicenseDefinition)**
- License types available for purchase
- Container of Features they unlock
- Product-specific licensing options
- Template for License Instances

**4. Product Instances (IProductInstanceInfo, CProductInstanceInfo)**
- Deployed product at customer site
- References parent Product
- Customer/account association
- Container of License Instances
- Hardware binding support

**5. License Instances (ILicenseInstance, CLicenseInstance)**
- Activated license with temporal control
- Based on License Definition
- Expiration date + goodwill period
- Cryptographic key management
- Validation status

**6. Hardware Instances (IHardwareInstanceInfo, CHardwareInstanceInfo)**
- Physical hardware identification
- Software-hardware binding
- Production tracking

#### Components (13 Total)

| Component | Purpose |
|-----------|---------|
| `CLicenseControllerComp` | Client-side license validation & import |
| `CProductControllerComp` | Controls product data persistence |
| `CFeatureInfoProviderComp` | Provides access to feature catalog |
| `CLicenseInfoProviderComp` | Provides access to license definitions |
| `CLicenseBasedRightsProviderComp` | Access control based on licenses |
| `CProductInfoComp` | Product information implementation |
| `CProductInstanceInfoComp` | Product instance implementation |
| `CProductLicensingInfoComp` | Complete licensing catalog |
| `CFeatureMetaInfoCreatorComp` | Feature metadata creation |
| `CLicenseMetaInfoCreatorComp` | License metadata creation |
| `CProductMetaInfoCreatorComp` | Product metadata creation |
| `CProductInstanceMetaInfoCreatorComp` | Product instance metadata |
| `CProductLicensingInfoMetaInfoCreatorComp` | Licensing catalog metadata |

#### Interfaces (20 Total)

Provider Interfaces:
- `IFeatureInfoProvider` - Feature catalog access
- `ILicenseInfoProvider` - License definition access
- `ILicenseInstanceProvider` - Activated license access
- `IProductInstanceInfoProvider` - Product instance access
- `IProductLicensingInfoProvider` - Licensing catalog access

Domain Interfaces:
- `IProductInfo` - Product definition
- `IFeatureInfo` - Feature capability
- `ILicenseDefinition` - License type
- `IProductInstanceInfo` - Deployed product
- `ILicenseInstance` - Activated license
- `IProductLicensingInfo` - Complete licensing info

Dependency Management:
- `IFeatureDependenciesManager/Provider` - Feature dependencies
- `ILicenseDependenciesManager/Provider` - License dependencies

Control Interfaces:
- `ILicenseController` - License file import/validation
- `ILicenseStatus` - License validation status

Hardware Interfaces:
- `IHardwareInstanceInfo` - Hardware instance info
- `ILicensedHardwareInstanceInfo` - Licensed hardware info

Security Interfaces:
- `IKeyRing` - Key management
- `IEncyptedLicenseKeyProvider` - Encrypted keys
- `IPartProductionInfo` - Production tracking

#### Existing Documentation
- `CONCEPTUAL_MODEL.md` - English conceptual overview
- `CONCEPTUAL_MODEL_RU.md` - Russian conceptual overview  
- `DOKUMENTATION_ZUSAMMENFASSUNG.md` - German documentation summary

---

### imtlicgql - GraphQL Interface

#### Purpose
GraphQL adapter layer providing RESTful API access to licensing functionality.

#### Key Abstractions

**1. Collection Controllers**
- `CFeatureCollectionControllerComp` - CRUD for feature collections
- `CLicenseCollectionControllerComp` - CRUD for license collections
- `CProductCollectionControllerComp` - CRUD for product collections
- `CAccountControllerComp` - CRUD for account information

Handle bulk operations, filtering, pagination, and synchronization.

**2. Object Controllers**
- `CLicenseControllerComp` - Single license operations
- `CFeatureControllerComp` - Single feature operations
- `CProductControllerComp` - Single product operations

Handle individual object CRUD operations.

**3. Data Providers**
- `CProductInfoProviderComp` - Product licensing data via GraphQL
- `CPermissionsProviderComp` - Feature-based permissions
- `CFeatureDependenciesProviderComp` - Feature dependency queries

**4. Metadata Delegates**
- `CProductMetaInfoDelegateComp` - Product JSON serialization
- `CFeatureMetaInfoDelegateComp` - Feature JSON serialization
- `CLicenseMetaInfoDelegateComp` - License JSON serialization

**5. Specialized Components**
- `CProductInfoFileGeneratorComp` - Generate C++ headers from product definitions
- `CProductPermissionsControllerComp` - Permission routing

#### GraphQL Operations Supported

**Queries:**
- List products/features/licenses with filtering
- Get individual object details
- Query feature dependencies
- Retrieve permissions
- Account information

**Mutations:**
- Create/Update/Delete products/features/licenses
- Import product definitions
- Update permissions
- Manage accounts

**Subscriptions:**
- Real-time updates on licensing changes (if supported by imtservergql)

#### Integration with imtlic
- Consumes imtlic types: `CFeatureInfo`, `ILicenseDefinition`, `IProductInfo`
- Converts between GraphQL representations and domain objects
- Uses imtlic factories for object instantiation
- Implements imtlic provider interfaces

---

### imtlicdb - Database Layer

#### Purpose
Database persistence layer for licensing data using PostgreSQL with JSON documents.

#### Key Abstractions

**Database Delegates (3 Components):**

1. **CLicenseDatabaseDelegateComp**
   - SQL queries for license retrieval
   - License filtering by ID and text search
   - Metadata extraction from JSON documents
   - Product reference validation

2. **CProductsDatabaseDelegateComp**
   - SQL queries for product retrieval
   - Product filtering by ID, category, text search
   - Metadata extraction from JSON documents
   - Creation date tracking via revision history

3. **CFeatureDatabaseDelegateComp**
   - SQL queries for feature retrieval
   - Feature metadata extraction
   - Feature name and ID indexing

#### Database Schema (JSON Documents)

**Licenses Table:**
```json
{
  "LicenseId": "string",
  "LicenseName": "string",
  "LicenseDescription": "string",
  "ProductId": "string",
  "Features": [...],
  "Metadata": {...}
}
```

**Products Table:**
```json
{
  "ProductId": "string",
  "ProductName": "string",
  "ProductDescription": "string",
  "CategoryId": "string",
  "Features": [...],
  "Metadata": {...}
}
```

**Features:**
```json
{
  "FeatureId": "string",
  "FeatureName": "string",
  "FeatureDescription": "string",
  "Optional": boolean,
  "Permission": boolean,
  "Dependencies": [...]
}
```

#### SQL Capabilities
- **Base Selection Query**: Active records with JSON document retrieval
- **ID Filtering**: Exact match on primary keys
- **Text Search**: ILIKE-based full-text search on names/descriptions
- **Join Queries**: Product references in licenses
- **Historical Data**: Revision-based time travel (RevisionNumber=1 for creation date)

#### Integration with imtdb
- Inherits from `CSqlJsonDatabaseDelegateComp`
- Uses PostgreSQL JSON operators (`->`, `->>`)
- Leverages imtdb transaction management
- Uses imtdb metadata infrastructure

---

## Design Patterns

### 1. Definition-Instance Pattern
Separates static definitions (Product, License Definition, Feature) from runtime instances (Product Instance, License Instance). This allows:
- One definition → many instances
- Centralized definition management
- Instance-specific customization (expiration dates, customer binding)

### 2. Container Pattern
Products and License Definitions are containers of Features:
- Product contains all available Features
- License Definition contains unlocked Features
- Hierarchical feature trees
- Dependency management between features

### 3. Provider Pattern
Data access through provider interfaces:
- Decouples data consumers from data sources
- Multiple implementations possible (database, file, cache)
- Dependency injection support

### 4. Component Pattern (ACF)
All major classes are ACF components:
- Factory-based instantiation
- Interface-based programming
- Dependency injection
- Configuration-driven composition

### 5. Adapter Pattern
imtlicgql adapts imtlic domain model to GraphQL:
- GraphQL schema → Domain objects
- SDL-based controllers
- Representation model conversion

### 6. Delegate Pattern
Database and metadata delegates:
- Separate persistence logic from domain logic
- JSON serialization delegation
- SQL query generation delegation

### 7. Template Method Pattern
Database delegates override base class methods:
- `GetBaseSelectionQuery()` - Custom SQL generation
- `CreateObjectFilterQuery()` - ID-based filtering
- `CreateTextFilterQuery()` - Full-text search
- `SetObjectMetaInfoFromRecord()` - Metadata extraction

---

## Data Flow

### License Activation Flow

```
1. Client Request (GraphQL)
   └─> imtlicgql: CLicenseCollectionControllerComp
       └─> imtlic: CLicenseControllerComp (validation)
           └─> imtlicdb: CLicenseDatabaseDelegateComp
               └─> PostgreSQL: INSERT license instance
               <── License record
           <── ILicenseInstance object
       <── GraphQL response
   <── JSON response to client
```

### Feature Query Flow

```
1. Client Request (GraphQL)
   └─> imtlicgql: CFeatureCollectionControllerComp
       └─> imtlic: CFeatureInfoProviderComp
           └─> imtlicdb: CFeatureDatabaseDelegateComp
               └─> PostgreSQL: SELECT features WHERE...
               <── Feature records (JSON)
           <── IFeatureInfo objects
       <── Feature collection
   <── GraphQL response (JSON)
```

### Product Import Flow

```
1. Client Upload (GraphQL mutation)
   └─> imtlicgql: CProductCollectionControllerComp
       └─> imtlic: CProductControllerComp
           ├─> Validate product definition
           ├─> Create feature objects
           └─> imtlicdb: CProductsDatabaseDelegateComp
               └─> PostgreSQL: INSERT product + features
               <── Product ID
           <── IProductInfo object
       <── Success response
   <── GraphQL response
```

---

## Key Concepts

### Feature-Based Licensing
The system uses features as the atomic unit of licensing:
- **Products** define available features
- **Licenses** specify which features they unlock
- **Instances** activate licenses to enable features

### Temporal Control
License Instances include temporal constraints:
- **Expiration Date**: When license expires
- **Goodwill Period**: Grace period after expiration
- **Validation Status**: Active, Expired, Grace period

### Product-Instance Separation
Clear separation between product definitions and deployments:
- **Product**: What can be sold (feature catalog)
- **Product Instance**: What was sold to a customer (with licenses)

### Hardware Binding
Products can be bound to hardware:
- Hardware Instance Info (physical identification)
- Licensed Hardware Instance Info (software-hardware link)
- Part Production Info (manufacturing data)

### Dependency Management
Features and licenses can have dependencies:
- **Feature Dependencies**: Feature A requires Feature B
- **License Dependencies**: License X requires License Y
- **Hierarchical Features**: Parent-child relationships

### Access Control
Feature-based permission system:
- **Permission Features**: Special features for access control
- **License-Based Rights**: Rights granted by active licenses
- **Optional Features**: Can be sold separately

---

## Integration Points

### With imtservergql
- Collection controller base classes
- GraphQL schema definitions
- Request/response handling
- Authentication/authorization

### With imtdb
- Database delegate base classes
- SQL query infrastructure
- Transaction management
- JSON document storage

### With imtbase
- Collection infrastructure
- Object factories
- Meta-information system
- Testing framework

### With imtgql
- GraphQL request parsing
- Query/mutation handling
- Variable substitution
- Schema validation

### With Client Applications
- GraphQL API endpoint
- JSON request/response format
- Authentication tokens
- WebSocket subscriptions (if supported)

---

## Summary

The ImtCore licensing system is a well-architected, three-layer solution:

1. **imtlic** - Domain model with rich business logic
2. **imtlicgql** - GraphQL API with modern web integration
3. **imtlicdb** - PostgreSQL persistence with JSON flexibility

The architecture supports:
- Flexible feature-based licensing
- Temporal license control
- Hardware binding
- Dependency management
- Access control
- Real-time GraphQL APIs
- Scalable database persistence

This design enables rapid development of licensing-aware applications while maintaining clean separation of concerns and architectural integrity.
