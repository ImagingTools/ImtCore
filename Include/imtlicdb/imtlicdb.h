// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


/**
	\namespace imtlicdb
	\brief Database persistence layer for licensing domain model using PostgreSQL with JSON documents.
	
	\section overview Overview
	
	The imtlicdb module provides database persistence for the imtlic licensing domain model.
	It implements database delegate components that handle SQL/JSON-based data storage and
	retrieval for licenses, products, and features. The module bridges the licensing domain
	(imtlic) with the database infrastructure (imtdb) using PostgreSQL's powerful JSON
	capabilities.
	
	\section architecture Architecture
	
	The module follows a delegate pattern architecture with three main components:
	
	**Database Delegates:**
	- **CLicenseDatabaseDelegateComp**: License data persistence
	- **CProductsDatabaseDelegateComp**: Product data persistence
	- **CFeatureDatabaseDelegateComp**: Feature data persistence
	
	All delegates inherit from CSqlJsonDatabaseDelegateComp (imtdb module), providing:
	- SQL query generation
	- JSON document handling
	- Object-relational mapping
	- Filtering and search capabilities
	- Metadata extraction
	
	\section design_patterns Design Patterns Used
	
	\subsection pattern_delegate Delegate Pattern
	Database operations are delegated to specialized components:
	- Each entity type (License, Product, Feature) has its own delegate
	- Delegates handle persistence concerns
	- Domain logic remains in imtlic module
	- Clean separation of concerns
	
	\subsection pattern_template_method Template Method Pattern
	Base class (CSqlJsonDatabaseDelegateComp) defines algorithm structure:
	- GetBaseSelectionQuery() - Override for custom SQL
	- CreateObjectFilterQuery() - Override for ID filtering
	- CreateTextFilterQuery() - Override for text search
	- SetObjectMetaInfoFromRecord() - Override for metadata extraction
	- SetCollectionItemMetaInfoFromRecord() - Override for collection items
	
	\subsection pattern_data_mapper Data Mapper Pattern
	Maps database records to domain objects:
	- JSON documents → C++ objects (deserialization)
	- C++ objects → JSON documents (serialization)
	- Attribute mapping and transformation
	- Type conversion and validation
	
	\subsection pattern_component Component Pattern (ACF)
	All delegates are ACF components:
	- Factory-based instantiation
	- Configuration-driven setup
	- Dependency injection support
	
	\section components Key Components
	
	\subsection comp_license_delegate CLicenseDatabaseDelegateComp
	
	**Purpose:** Manages database operations for license definitions
	
	**Key Responsibilities:**
	- Store and retrieve license definitions
	- Filter licenses by ID, name, product
	- Full-text search on license names and descriptions
	- Extract license metadata from JSON documents
	- Validate product references
	
	**SQL Queries:**
	```sql
	-- Base selection query
	SELECT Id, DocumentId, RevisionNumber, Document, 
	       (Document->>'LicenseId') as LicenseId,
	       (Document->>'LicenseName') as LicenseName,
	       (Document->>'ProductId') as ProductId
	FROM Licenses
	WHERE RevisionNumber = 0  -- Active records only
	
	-- ID filter query
	WHERE (Document->>'LicenseId') = ?
	
	-- Text search query
	WHERE (Document->>'LicenseName') ILIKE ?
	   OR (Document->>'LicenseDescription') ILIKE ?
	```
	
	**JSON Document Structure:**
	```json
	{
	  "LicenseId": "license-uuid",
	  "LicenseName": "Professional License",
	  "LicenseDescription": "Full-featured professional license",
	  "ProductId": "product-uuid",
	  "Features": [
	    {"FeatureId": "feature-1", "FeatureName": "Advanced Analytics"},
	    {"FeatureId": "feature-2", "FeatureName": "Export Functionality"}
	  ],
	  "Dependencies": ["license-uuid-base"],
	  "Metadata": {...}
	}
	```
	
	**Cross-Table Operations:**
	- Joins with Products table for product validation
	- Queries product creation dates via RevisionNumber=1
	
	\subsection comp_products_delegate CProductsDatabaseDelegateComp
	
	**Purpose:** Manages database operations for product definitions
	
	**Key Responsibilities:**
	- Store and retrieve product definitions
	- Filter products by ID, category, name
	- Full-text search on product names and descriptions
	- Extract product metadata from JSON documents
	- Track product creation dates
	
	**SQL Queries:**
	```sql
	-- Base selection query
	SELECT Id, DocumentId, RevisionNumber, Document,
	       (Document->>'ProductId') as ProductId,
	       (Document->>'ProductName') as ProductName,
	       (Document->>'CategoryId') as CategoryId
	FROM Products
	WHERE RevisionNumber = 0  -- Active records only
	
	-- Category filter query
	WHERE (Document->>'CategoryId') = ?
	
	-- Text search query
	WHERE (Document->>'ProductName') ILIKE ?
	   OR (Document->>'ProductDescription') ILIKE ?
	```
	
	**JSON Document Structure:**
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
	  "Metadata": {...}
	}
	```
	
	**Historical Data:**
	- RevisionNumber=0: Current active record
	- RevisionNumber=1: Initial creation record (for creation date)
	- RevisionNumber>1: Historical versions
	
	\subsection comp_feature_delegate CFeatureDatabaseDelegateComp
	
	**Purpose:** Manages database operations for feature definitions
	
	**Key Responsibilities:**
	- Store and retrieve feature definitions
	- Filter features by ID and name
	- Extract feature metadata from JSON documents
	- Handle feature hierarchies (parent-child)
	- Manage feature dependencies
	
	**SQL Queries:**
	```sql
	-- Base selection query
	SELECT Id, DocumentId, RevisionNumber, Document,
	       (Document->>'FeatureId') as FeatureId,
	       (Document->>'FeatureName') as FeatureName
	FROM Features
	WHERE RevisionNumber = 0  -- Active records only
	
	-- ID filter query
	WHERE (Document->>'FeatureId') = ?
	
	-- Name filter query
	WHERE (Document->>'FeatureName') = ?
	```
	
	**JSON Document Structure:**
	```json
	{
	  "FeatureId": "feature-uuid",
	  "FeatureName": "Advanced Analytics",
	  "FeatureDescription": "Data analytics and reporting",
	  "Optional": true,
	  "Permission": false,
	  "ParentFeatureId": "parent-feature-uuid",
	  "Dependencies": [
	    {
	      "FeatureId": "dependency-feature-uuid",
	      "Required": true
	    }
	  ],
	  "Metadata": {...}
	}
	```
	
	\section database_schema Database Schema
	
	\subsection schema_tables Table Structure
	
	The module uses three main tables in PostgreSQL:
	
	**Licenses Table:**
	- `Id` (SERIAL PRIMARY KEY): Internal database ID
	- `DocumentId` (UUID): Logical document identifier
	- `RevisionNumber` (INTEGER): Version tracking (0 = active)
	- `Document` (JSONB): Complete license definition as JSON
	- `CreatedAt` (TIMESTAMP): Record creation time
	- `UpdatedAt` (TIMESTAMP): Last update time
	
	**Products Table:**
	- `Id` (SERIAL PRIMARY KEY): Internal database ID
	- `DocumentId` (UUID): Logical document identifier
	- `RevisionNumber` (INTEGER): Version tracking (0 = active)
	- `Document` (JSONB): Complete product definition as JSON
	- `CreatedAt` (TIMESTAMP): Record creation time
	- `UpdatedAt` (TIMESTAMP): Last update time
	
	**Features Table:**
	- `Id` (SERIAL PRIMARY KEY): Internal database ID
	- `DocumentId` (UUID): Logical document identifier
	- `RevisionNumber` (INTEGER): Version tracking (0 = active)
	- `Document` (JSONB): Complete feature definition as JSON
	- `CreatedAt` (TIMESTAMP): Record creation time
	- `UpdatedAt` (TIMESTAMP): Last update time
	
	\subsection schema_indexes Index Strategy
	
	**Recommended Indexes:**
	```sql
	-- Active record indexes
	CREATE INDEX idx_licenses_active ON Licenses(DocumentId, RevisionNumber);
	CREATE INDEX idx_products_active ON Products(DocumentId, RevisionNumber);
	CREATE INDEX idx_features_active ON Features(DocumentId, RevisionNumber);
	
	-- JSON field indexes using PostgreSQL GIN indexes
	CREATE INDEX idx_licenses_license_id ON Licenses USING GIN ((Document->'LicenseId'));
	CREATE INDEX idx_licenses_product_id ON Licenses USING GIN ((Document->'ProductId'));
	CREATE INDEX idx_products_product_id ON Products USING GIN ((Document->'ProductId'));
	CREATE INDEX idx_products_category_id ON Products USING GIN ((Document->'CategoryId'));
	CREATE INDEX idx_features_feature_id ON Features USING GIN ((Document->'FeatureId'));
	
	-- Full-text search indexes
	CREATE INDEX idx_licenses_name ON Licenses USING GIN (to_tsvector('english', Document->>'LicenseName'));
	CREATE INDEX idx_products_name ON Products USING GIN (to_tsvector('english', Document->>'ProductName'));
	```
	
	\section sql_capabilities SQL and JSON Capabilities
	
	\subsection sql_json_operators PostgreSQL JSON Operators
	
	The module leverages PostgreSQL's JSON operators:
	
	**Arrow Operators:**
	- `Document->'FieldName'` - Returns JSON object (keeps quotes)
	- `Document->>'FieldName'` - Returns text value (removes quotes)
	
	**Path Operators:**
	- `Document#>'{Features,0,FeatureId}'` - Navigate nested JSON
	- `Document#>>'{Features,0,FeatureName}'` - Navigate and extract text
	
	**Containment Operators:**
	- `Document @> '{"ProductId": "uuid"}'` - Contains JSON object
	- `Document ? 'FieldName'` - Has key
	
	\subsection sql_filtering Filtering Capabilities
	
	**ID-Based Filtering:**
	```cpp
	// CLicenseDatabaseDelegateComp::CreateObjectFilterQuery()
	QString filter = QString("(Document->>'LicenseId') = '%1'").arg(licenseId);
	```
	
	**Text Search (ILIKE):**
	```cpp
	// CProductsDatabaseDelegateComp::CreateTextFilterQuery()
	QString filter = QString(
	    "(Document->>'ProductName') ILIKE '%%1%' OR "
	    "(Document->>'ProductDescription') ILIKE '%%1%'"
	).arg(searchText);
	```
	
	**Category Filtering:**
	```cpp
	// CProductsDatabaseDelegateComp - Category filter
	QString filter = QString("(Document->>'CategoryId') = '%1'").arg(categoryId);
	```
	
	**Compound Filtering:**
	```cpp
	// Multiple conditions combined
	QString filter = QString(
	    "(Document->>'ProductId') = '%1' AND "
	    "(Document->>'CategoryId') = '%2' AND "
	    "RevisionNumber = 0"
	).arg(productId, categoryId);
	```
	
	\subsection sql_joins Cross-Table Queries
	
	**License-Product Join:**
	```sql
	SELECT 
	    l.Document as LicenseDocument,
	    p.Document as ProductDocument,
	    (SELECT Document->>'CreatedAt' FROM Products 
	     WHERE DocumentId = p.DocumentId AND RevisionNumber = 1) as ProductCreatedAt
	FROM Licenses l
	JOIN Products p ON (l.Document->>'ProductId') = (p.Document->>'ProductId')
	WHERE l.RevisionNumber = 0 AND p.RevisionNumber = 0
	```
	
	\section data_flow Data Flow Examples
	
	\subsection flow_store_product Storing a Product
	
	```
	1. imtlicgql: CProductCollectionControllerComp
	   └─> GraphQL mutation received
	       └─> imtlic: CProductControllerComp
	           └─> Create IProductInfo object
	           └─> imtlicdb: CProductsDatabaseDelegateComp
	               ├─> Convert IProductInfo to JSON document
	               ├─> Generate SQL INSERT statement
	               └─> PostgreSQL: INSERT INTO Products (DocumentId, RevisionNumber, Document)
	                              VALUES (uuid, 0, jsonb_document)
	               <── Product ID (DocumentId)
	           <── IProductInfo with ID
	       <── GraphQL response
	   <── Success
	```
	
	\subsection flow_query_licenses Querying Licenses
	
	```
	1. imtlicgql: CLicenseCollectionControllerComp
	   └─> GraphQL query received with filters
	       └─> imtlic: CLicenseInfoProviderComp
	           └─> imtlicdb: CLicenseDatabaseDelegateComp
	               ├─> GetBaseSelectionQuery() - Build base SQL
	               ├─> CreateObjectFilterQuery() - Add ID filter
	               ├─> CreateTextFilterQuery() - Add text search
	               └─> PostgreSQL: SELECT ... FROM Licenses WHERE ...
	               <── Result set (JSON documents)
	               ├─> SetObjectMetaInfoFromRecord() - Parse each record
	               ├─> Create ILicenseDefinition objects
	               └─> Populate with metadata
	               <── List of ILicenseDefinition objects
	           <── License collection
	       <── GraphQL response
	   <── JSON license list
	```
	
	\subsection flow_feature_lookup Feature Lookup by ID
	
	```
	1. Application needs feature details
	   └─> imtlic: CFeatureInfoProviderComp
	       └─> imtlicdb: CFeatureDatabaseDelegateComp
	           ├─> CreateObjectFilterQuery("feature-uuid")
	           └─> PostgreSQL: SELECT * FROM Features 
	                          WHERE (Document->>'FeatureId') = 'feature-uuid' 
	                          AND RevisionNumber = 0
	           <── Single JSON document
	           ├─> SetObjectMetaInfoFromRecord()
	           ├─> Extract FeatureId, FeatureName, etc.
	           └─> Create IFeatureInfo object
	           <── IFeatureInfo object
	       <── Feature details
	   <── Feature available for use
	```
	
	\section integration Integration with Other Modules
	
	\subsection integration_imtdb Integration with imtdb (Database Framework)
	
	**Extends Base Classes:**
	- CSqlJsonDatabaseDelegateComp - Base delegate with SQL/JSON support
	- Inherits transaction management
	- Inherits connection pooling
	- Inherits query execution infrastructure
	
	**Uses imtdb Features:**
	- JSON document storage and retrieval
	- Active record pattern (RevisionNumber = 0)
	- Historical versioning support
	- Metadata infrastructure
	- Query builders and filters
	
	\subsection integration_imtlic Integration with imtlic (Licensing Core)
	
	**Implements Persistence For:**
	- IProductInfo / CProductInfo
	- IFeatureInfo / CFeatureInfo
	- ILicenseDefinition / CLicenseDefinition
	
	**Data Flow:**
	- imtlic creates domain objects
	- imtlicdb persists domain objects
	- imtlic queries domain objects
	- imtlicdb retrieves and reconstructs objects
	
	**No Direct Coupling:**
	- imtlicdb has no references to imtlic headers
	- Uses metadata and JSON for serialization
	- Domain logic remains in imtlic
	
	\subsection integration_imtlicgql Integration with imtlicgql (GraphQL API)
	
	**Indirect Integration:**
	- imtlicgql → imtlic → imtlicdb
	- No direct calls from imtlicgql to imtlicdb
	- Clean layered architecture maintained
	
	**Data Flow:**
	```
	GraphQL Request → imtlicgql → imtlic → imtlicdb → PostgreSQL
	PostgreSQL → imtlicdb → imtlic → imtlicgql → GraphQL Response
	```
	
	\section performance Performance Considerations
	
	\subsection perf_indexing Indexing Strategy
	- Create indexes on frequently queried JSON fields
	- Use GIN indexes for JSON containment queries
	- Use B-tree indexes for equality comparisons
	- Consider full-text search indexes for text fields
	
	\subsection perf_querying Query Optimization
	- Use active record pattern (RevisionNumber = 0) to reduce result set
	- Limit result sets with LIMIT/OFFSET for pagination
	- Use prepared statements for repeated queries
	- Batch operations when possible
	
	\subsection perf_caching Caching Strategies
	- Cache frequently accessed products/features
	- Invalidate cache on updates
	- Use Redis or memcached for distributed caching
	- Consider read replicas for query load distribution
	
	\subsection perf_json JSON Document Size
	- Keep JSON documents reasonably sized
	- Avoid deeply nested structures
	- Consider normalizing large arrays
	- Use JSONB for better performance vs JSON
	
	\section best_practices Best Practices
	
	\subsection best_transactions Transaction Management
	- Use transactions for multi-table operations
	- Keep transactions short and focused
	- Handle rollback scenarios
	- Avoid long-running transactions
	
	\subsection best_versioning Version Control
	- Use RevisionNumber for version tracking
	- Keep RevisionNumber=0 as active record
	- Maintain historical records for audit trail
	- Consider retention policies for old revisions
	
	\subsection best_validation Data Validation
	- Validate JSON structure before insertion
	- Check foreign key references (ProductId in Licenses)
	- Enforce required fields
	- Validate data types and formats
	
	\subsection best_security Security Best Practices
	- Use parameterized queries to prevent SQL injection
	- Validate all input data
	- Implement row-level security if needed
	- Audit sensitive operations
	- Encrypt sensitive fields in JSON documents
	
	\section documentation Additional Documentation
	
	- **LICENSING_ARCHITECTURE.md**: Complete three-module architecture
	- **imtlic module documentation**: Core domain model
	- **imtdb module documentation**: Database framework
	- **PostgreSQL JSON documentation**: JSON operators and functions
	
	\see imtlic For core licensing domain model
	\see imtlicgql For GraphQL API layer
	\see imtdb For database framework infrastructure
	\ingroup LicenseManagement
*/
namespace imtlicdb
{


} // namespace imtlicdb


