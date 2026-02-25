# imtlicdb Module

## Overview

The **imtlicdb** module provides database persistence for the imtlic licensing domain model. It implements database delegate components that handle SQL/JSON-based data storage and retrieval for licenses, products, and features using PostgreSQL's powerful JSON capabilities.

## Key Features

- **PostgreSQL JSON Storage**: Flexible document-based storage with SQL queries
- **SQL/JSON Queries**: Leverages PostgreSQL JSON operators for efficient queries
- **Active Record Pattern**: Version tracking with RevisionNumber (0 = active)
- **Full-Text Search**: ILIKE-based search on names and descriptions
- **Cross-Table Joins**: Product references and relationship queries
- **Historical Versioning**: Maintains record history for audit trails
- **Object-Relational Mapping**: JSON documents ↔ C++ objects
- **Filtering & Search**: ID-based, category-based, and text-based filtering

## Architecture

### Database Delegates

The module implements three database delegate components:

| Component | Purpose | Table |
|-----------|---------|-------|
| `CLicenseDatabaseDelegateComp` | License data persistence | Licenses |
| `CProductsDatabaseDelegateComp` | Product data persistence | Products |
| `CFeatureDatabaseDelegateComp` | Feature data persistence | Features |

All delegates inherit from `CSqlJsonDatabaseDelegateComp` (imtdb module), providing:
- SQL query generation
- JSON document handling
- Transaction management
- Metadata extraction

### Design Patterns

**Delegate Pattern:**
- Separates persistence logic from domain logic
- Each entity type has specialized delegate
- Clean separation of concerns

**Template Method Pattern:**
- Base class defines algorithm structure
- Subclasses override specific methods:
  - `GetBaseSelectionQuery()` - Custom SQL generation
  - `CreateObjectFilterQuery()` - ID-based filtering
  - `CreateTextFilterQuery()` - Full-text search
  - `SetObjectMetaInfoFromRecord()` - Metadata extraction

**Data Mapper Pattern:**
- Maps database records to domain objects
- JSON documents ↔ C++ objects
- Type conversion and validation

## Database Schema

### Table Structure

**Licenses Table:**
```sql
CREATE TABLE Licenses (
    Id SERIAL PRIMARY KEY,
    DocumentId UUID NOT NULL,
    RevisionNumber INTEGER NOT NULL,
    Document JSONB NOT NULL,
    CreatedAt TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    UpdatedAt TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);
```

**Products Table:**
```sql
CREATE TABLE Products (
    Id SERIAL PRIMARY KEY,
    DocumentId UUID NOT NULL,
    RevisionNumber INTEGER NOT NULL,
    Document JSONB NOT NULL,
    CreatedAt TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    UpdatedAt TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);
```

**Features Table:**
```sql
CREATE TABLE Features (
    Id SERIAL PRIMARY KEY,
    DocumentId UUID NOT NULL,
    RevisionNumber INTEGER NOT NULL,
    Document JSONB NOT NULL,
    CreatedAt TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    UpdatedAt TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);
```

### JSON Document Structure

**License Document:**
```json
{
  "LicenseId": "license-uuid",
  "LicenseName": "Professional License",
  "LicenseDescription": "Full-featured professional edition",
  "ProductId": "product-uuid",
  "Features": [
    {
      "FeatureId": "feature-1",
      "FeatureName": "Advanced Analytics"
    },
    {
      "FeatureId": "feature-2",
      "FeatureName": "PDF Export"
    }
  ],
  "Dependencies": ["license-basic-uuid"],
  "Metadata": {
    "CreatedBy": "admin",
    "CreatedAt": "2024-01-01T00:00:00Z"
  }
}
```

**Product Document:**
```json
{
  "ProductId": "product-uuid",
  "ProductName": "Enterprise Suite",
  "ProductDescription": "Complete enterprise solution",
  "CategoryId": "software-category",
  "Features": [
    {
      "FeatureId": "feature-1",
      "FeatureName": "Core Functionality",
      "Optional": false,
      "Permission": false
    },
    {
      "FeatureId": "feature-2",
      "FeatureName": "Advanced Features",
      "Optional": true,
      "Permission": false
    }
  ],
  "Metadata": {}
}
```

**Feature Document:**
```json
{
  "FeatureId": "feature-uuid",
  "FeatureName": "Advanced Analytics",
  "FeatureDescription": "Data analysis and reporting",
  "Optional": true,
  "Permission": false,
  "ParentFeatureId": "parent-feature-uuid",
  "Dependencies": [
    {
      "FeatureId": "dependency-feature-uuid",
      "Required": true
    }
  ],
  "Metadata": {}
}
```

### Indexing Strategy

**Active Record Indexes:**
```sql
CREATE INDEX idx_licenses_active ON Licenses(DocumentId, RevisionNumber);
CREATE INDEX idx_products_active ON Products(DocumentId, RevisionNumber);
CREATE INDEX idx_features_active ON Features(DocumentId, RevisionNumber);
```

**JSON Field Indexes (GIN):**
```sql
CREATE INDEX idx_licenses_license_id ON Licenses USING GIN ((Document->'LicenseId'));
CREATE INDEX idx_licenses_product_id ON Licenses USING GIN ((Document->'ProductId'));
CREATE INDEX idx_products_product_id ON Products USING GIN ((Document->'ProductId'));
CREATE INDEX idx_products_category_id ON Products USING GIN ((Document->'CategoryId'));
CREATE INDEX idx_features_feature_id ON Features USING GIN ((Document->'FeatureId'));
```

**Full-Text Search Indexes:**
```sql
CREATE INDEX idx_licenses_name ON Licenses 
  USING GIN (to_tsvector('english', Document->>'LicenseName'));
CREATE INDEX idx_products_name ON Products 
  USING GIN (to_tsvector('english', Document->>'ProductName'));
```

## SQL Capabilities

### PostgreSQL JSON Operators

**Arrow Operators:**
- `Document->'FieldName'` - Returns JSON object (with quotes)
- `Document->>'FieldName'` - Returns text value (without quotes)

**Path Operators:**
- `Document#>'{Features,0,FeatureId}'` - Navigate nested JSON
- `Document#>>'{Features,0,FeatureName}'` - Navigate and extract text

**Containment Operators:**
- `Document @> '{"ProductId": "uuid"}'` - Contains JSON object
- `Document ? 'FieldName'` - Has key

### Query Examples

**Base Selection Query (Active Records):**
```sql
SELECT Id, DocumentId, RevisionNumber, Document,
       (Document->>'LicenseId') as LicenseId,
       (Document->>'LicenseName') as LicenseName,
       (Document->>'ProductId') as ProductId
FROM Licenses
WHERE RevisionNumber = 0;  -- Active records only
```

**ID-Based Filtering:**
```sql
SELECT * FROM Licenses
WHERE (Document->>'LicenseId') = 'license-uuid-123'
  AND RevisionNumber = 0;
```

**Text Search (ILIKE):**
```sql
SELECT * FROM Products
WHERE ((Document->>'ProductName') ILIKE '%Office%'
   OR (Document->>'ProductDescription') ILIKE '%Office%')
  AND RevisionNumber = 0;
```

**Category Filtering:**
```sql
SELECT * FROM Products
WHERE (Document->>'CategoryId') = 'software-category'
  AND RevisionNumber = 0;
```

**License-Product Join:**
```sql
SELECT 
    l.Document as LicenseDocument,
    p.Document as ProductDocument,
    (SELECT Document->>'CreatedAt' 
     FROM Products 
     WHERE DocumentId = p.DocumentId AND RevisionNumber = 1) as ProductCreatedAt
FROM Licenses l
JOIN Products p ON (l.Document->>'ProductId') = (p.Document->>'ProductId')
WHERE l.RevisionNumber = 0 AND p.RevisionNumber = 0;
```

## Component Details

### CLicenseDatabaseDelegateComp

**Purpose:** Manages database operations for license definitions

**Key Methods:**
- `GetBaseSelectionQuery()` - Returns base SELECT statement for licenses
- `CreateObjectFilterQuery(licenseId)` - Filter by license ID
- `CreateTextFilterQuery(searchText)` - Full-text search on license name/description
- `SetObjectMetaInfoFromRecord(record)` - Extract license metadata from JSON

**Example Usage:**
```cpp
CLicenseDatabaseDelegateCompSharedPtr delegate = ...;

// Query all licenses
QVector<ILicenseDefinition*> licenses = delegate->QueryAll();

// Filter by license ID
QString filter = delegate->CreateObjectFilterQuery("license-123");
QVector<ILicenseDefinition*> filtered = delegate->QueryFiltered(filter);

// Text search
QString search = delegate->CreateTextFilterQuery("Professional");
QVector<ILicenseDefinition*> results = delegate->QueryFiltered(search);
```

### CProductsDatabaseDelegateComp

**Purpose:** Manages database operations for product definitions

**Key Methods:**
- `GetBaseSelectionQuery()` - Returns base SELECT statement for products
- `CreateObjectFilterQuery(productId)` - Filter by product ID
- `CreateTextFilterQuery(searchText)` - Full-text search on product name/description
- `SetObjectMetaInfoFromRecord(record)` - Extract product metadata from JSON

**Special Features:**
- Queries creation date via RevisionNumber=1 records
- Supports category filtering
- Cross-references with licenses table

**Example Usage:**
```cpp
CProductsDatabaseDelegateCompSharedPtr delegate = ...;

// Query all products
QVector<IProductInfo*> products = delegate->QueryAll();

// Filter by category
QString categoryFilter = "(Document->>'CategoryId') = 'software'";
QVector<IProductInfo*> categoryProducts = delegate->QueryFiltered(categoryFilter);

// Get product creation date
QString creationDateQuery = delegate->GetCreationDateQuery(productId);
QDateTime created = delegate->QueryCreationDate(creationDateQuery);
```

### CFeatureDatabaseDelegateComp

**Purpose:** Manages database operations for feature definitions

**Key Methods:**
- `GetBaseSelectionQuery()` - Returns base SELECT statement for features
- `CreateObjectFilterQuery(featureId)` - Filter by feature ID
- `SetObjectMetaInfoFromRecord(record)` - Extract feature metadata from JSON

**Special Features:**
- Handles feature hierarchies (parent-child)
- Manages feature dependencies
- Supports feature name filtering

**Example Usage:**
```cpp
CFeatureDatabaseDelegateCompSharedPtr delegate = ...;

// Query all features
QVector<IFeatureInfo*> features = delegate->QueryAll();

// Filter by feature ID
QString filter = delegate->CreateObjectFilterQuery("feature-analytics");
QVector<IFeatureInfo*> feature = delegate->QueryFiltered(filter);

// Filter by name
QString nameFilter = "(Document->>'FeatureName') = 'Advanced Analytics'";
QVector<IFeatureInfo*> named = delegate->QueryFiltered(nameFilter);
```

## Data Flow Examples

### Storing a Product

```
1. Application creates IProductInfo
   └─> imtlicgql: GraphQL mutation
       └─> imtlic: CProductControllerComp
           └─> imtlicdb: CProductsDatabaseDelegateComp
               ├─> Convert IProductInfo to JSON
               ├─> Generate INSERT statement
               └─> PostgreSQL: 
                   INSERT INTO Products (DocumentId, RevisionNumber, Document)
                   VALUES (uuid, 0, jsonb_document)
               <── DocumentId (Product ID)
           <── IProductInfo with ID
       <── Success
   <── Product created
```

### Querying Licenses

```
1. Application needs licenses
   └─> imtlicgql: GraphQL query
       └─> imtlic: CLicenseInfoProviderComp
           └─> imtlicdb: CLicenseDatabaseDelegateComp
               ├─> GetBaseSelectionQuery()
               ├─> CreateObjectFilterQuery() (if filtered)
               ├─> CreateTextFilterQuery() (if searching)
               └─> PostgreSQL:
                   SELECT ... FROM Licenses WHERE ...
               <── JSON documents
               ├─> SetObjectMetaInfoFromRecord() for each
               ├─> Create ILicenseDefinition objects
               └─> Populate with metadata
               <── List of ILicenseDefinition
           <── License collection
       <── GraphQL response
   <── License list
```

### Feature Lookup by ID

```
1. Application needs feature details
   └─> imtlic: CFeatureInfoProviderComp
       └─> imtlicdb: CFeatureDatabaseDelegateComp
           ├─> CreateObjectFilterQuery("feature-uuid")
           └─> PostgreSQL:
               SELECT * FROM Features
               WHERE (Document->>'FeatureId') = 'feature-uuid'
                 AND RevisionNumber = 0
           <── Single JSON document
           ├─> SetObjectMetaInfoFromRecord()
           ├─> Extract FeatureId, FeatureName, etc.
           └─> Create IFeatureInfo object
           <── IFeatureInfo
       <── Feature details
   <── Feature available
```

## Performance Optimization

### Query Optimization
- Use `RevisionNumber = 0` filter to reduce result sets
- Apply LIMIT/OFFSET for pagination
- Use prepared statements for repeated queries
- Batch operations when possible
- Leverage indexes on frequently queried fields

### Caching Strategies
- Cache frequently accessed products/features
- Invalidate cache on updates
- Use Redis or memcached for distributed caching
- Consider read replicas for query load

### JSON Document Size
- Keep documents reasonably sized
- Avoid deeply nested structures
- Consider normalizing large arrays
- Use JSONB for better performance

### Connection Pooling
- Reuse database connections
- Configure appropriate pool size
- Monitor connection usage
- Handle connection failures gracefully

## Best Practices

### Transaction Management
- Use transactions for multi-table operations
- Keep transactions short and focused
- Handle rollback scenarios properly
- Avoid long-running transactions

### Version Control
- Use RevisionNumber for version tracking
- Keep RevisionNumber=0 as active record
- Maintain historical records for audit
- Consider retention policies

### Data Validation
- Validate JSON structure before insertion
- Check foreign key references
- Enforce required fields
- Validate data types and formats

### Security
- Use parameterized queries to prevent SQL injection
- Validate all input data
- Implement row-level security if needed
- Audit sensitive operations
- Encrypt sensitive fields in JSON

## Integration

### With imtlic Module
- Implements persistence for IProductInfo, IFeatureInfo, ILicenseDefinition
- Domain objects created by imtlic, persisted by imtlicdb
- No direct coupling - uses metadata and JSON

### With imtdb Framework
- Extends CSqlJsonDatabaseDelegateComp
- Uses PostgreSQL JSON operators
- Leverages transaction management
- Uses query builders and filters

### With imtlicgql Module
- Indirect integration via imtlic
- imtlicgql → imtlic → imtlicdb → PostgreSQL
- Clean layered architecture

## Documentation

- **imtlicdb.h**: Comprehensive namespace documentation
- **LICENSING_ARCHITECTURE.md**: Complete architecture (root directory)
- **imtlic module**: Core domain model
- **imtdb module**: Database framework
- **PostgreSQL JSON docs**: JSON operators and functions

## Building

### CMake
```bash
cd Include/imtlicdb/CMake
cmake .
make
```

### QMake
```bash
cd Include/imtlicdb/QMake
qmake
make
```

## Related Modules

- **imtlic**: Core licensing domain model
- **imtlicgql**: GraphQL API layer
- **imtdb**: Database framework with SQL/JSON support
- **PostgreSQL**: Database engine with JSONB support

## License

See LICENSE file in repository root. ImtCore uses multi-licensing:
- LGPL-2.1-or-later
- GPL-2.0-or-later
- GPL-3.0-or-later
- LicenseRef-ImtCore-Commercial
