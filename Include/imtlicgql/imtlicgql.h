// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


/**
	\namespace imtlicgql
	\brief GraphQL API interface layer for the licensing domain model.
	
	\section overview Overview
	
	The imtlicgql module provides a GraphQL-based web API layer over the imtlic licensing
	domain model. It exposes licensing functionality (Products, Features, Licenses, Accounts)
	through standardized GraphQL operations, enabling modern web applications, mobile apps,
	and other clients to query and manipulate licensing data.
	
	\section architecture Architecture
	
	The module follows a three-tier adapter architecture:
	
	**Tier 1: Collection Controllers** (Bulk operations)
	- Handle collections of objects (Products, Features, Licenses, Accounts)
	- Provide CRUD operations (Create, Read, Update, Delete)
	- Support filtering, pagination, and search
	- Synchronize related collections (Products ↔ Licenses ↔ Features)
	
	**Tier 2: Object Controllers** (Single object operations)
	- Handle individual object operations
	- Simpler interface for single-item CRUD
	- Delegate to collection controllers or direct data access
	
	**Tier 3: Data Providers & Delegates** (Data access and transformation)
	- Data Providers: Query handling and data access
	- Metadata Delegates: JSON serialization/deserialization
	- Dependency Providers: Feature and license dependency resolution
	- Permission Providers: Feature-based access control
	
	\section design_patterns Design Patterns Used
	
	\subsection pattern_adapter Adapter Pattern
	The module adapts the imtlic domain model to GraphQL:
	- GraphQL SDL schemas → imtlic domain objects
	- Bidirectional conversion (GraphQL representations ↔ C++ objects)
	- Type mapping between GraphQL types and C++ classes
	
	\subsection pattern_component Component Pattern (ACF)
	All controllers are ACF components:
	- Factory-based instantiation
	- Interface-based programming
	- Dependency injection via component properties
	- Configuration-driven composition
	
	\subsection pattern_delegate Delegate Pattern
	Metadata delegates handle serialization:
	- CFeatureMetaInfoDelegateComp: Feature ↔ JSON
	- CLicenseMetaInfoDelegateComp: License ↔ JSON
	- CProductMetaInfoDelegateComp: Product ↔ JSON
	- Separate persistence concerns from business logic
	
	\subsection pattern_provider Provider Pattern
	Data providers abstract query handling:
	- CProductInfoProviderComp: Product licensing information
	- CPermissionsProviderComp: Feature-based permissions
	- CFeatureDependenciesProviderComp: Feature dependency queries
	- Decouple GraphQL handlers from data sources
	
	\section components Key Components
	
	\subsection comp_collection_controllers Collection Controllers
	
	**CFeatureCollectionControllerComp**
	- GraphQL handler for feature collections
	- SDL-based controller with representation mapping
	- Synchronizes with ProductCollection and LicenseCollection
	- Supports hierarchical feature trees
	- Handles feature dependencies
	
	**CLicenseCollectionControllerComp**
	- GraphQL handler for license definition collections
	- Converts between GraphQL representations and CLicenseDefinition objects
	- Synchronizes with ProductCollection and FeatureCollection
	- Handles license dependencies
	- Supports filtering by product
	
	**CProductCollectionControllerComp**
	- GraphQL handler for product collections
	- Import/export functionality for product definitions
	- MIME type handling for product files
	- Complete product lifecycle management
	- Synchronizes with LicenseCollection and FeatureCollection
	
	**CAccountControllerComp**
	- Collection controller for company/account information
	- Manages customer accounts and organizations
	- Links products to accounts
	
	\subsection comp_object_controllers Object Controllers
	
	**CLicenseControllerComp**
	- Individual license object controller
	- Single license CRUD operations
	- Delegates to CLicenseCollectionControllerComp
	
	**CFeatureControllerComp**
	- Individual feature object controller
	- Single feature CRUD operations
	- Delegates to CFeatureCollectionControllerComp
	
	**CProductControllerComp**
	- Individual product object controller
	- Single product CRUD operations
	- Delegates to CProductCollectionControllerComp
	
	\subsection comp_data_providers Data Providers
	
	**CProductInfoProviderComp**
	- Provides product licensing information via GraphQL
	- Access to complete product catalog
	- License availability queries
	- Feature availability queries
	
	**CPermissionsProviderComp**
	- Exposes feature-based permission models
	- Access control queries
	- Permission validation for features
	- Integration with license-based rights
	
	**CFeatureDependenciesProviderComp**
	- Manages feature dependency queries
	- Dependency graph navigation
	- Transitive dependency resolution
	- Implements IFeatureDependenciesProvider from imtlic
	
	**CProductPermissionsControllerComp**
	- Routes permission queries via ProductProvider
	- GraphQL endpoint for permission checks
	
	\subsection comp_metadata_delegates Metadata Delegates
	
	**CProductMetaInfoDelegateComp**
	- JSON-based metadata mapping for products
	- Serialization: IProductInfo → JSON
	- Deserialization: JSON → IProductInfo
	- Handles product properties and feature lists
	
	**CFeatureMetaInfoDelegateComp**
	- JSON-based metadata mapping for features
	- Serialization: IFeatureInfo → JSON
	- Deserialization: JSON → IFeatureInfo
	- Handles feature hierarchies and dependencies
	
	**CLicenseMetaInfoDelegateComp**
	- JSON-based metadata mapping for licenses
	- Serialization: ILicenseDefinition → JSON
	- Deserialization: JSON → ILicenseDefinition
	- Handles license features and dependencies
	
	\subsection comp_specialized Specialized Components
	
	**CProductInfoFileGeneratorComp**
	- Generates C++ header files from product definitions
	- Creates compile-time product constants
	- Generates feature ID enumerations
	- Useful for embedded systems and static linking
	
	\section graphql_operations GraphQL Operations
	
	\subsection gql_queries Queries
	
	**Collection Queries:**
	```graphql
	query {
	  products(filter: {...}, limit: 10, offset: 0) {
	    items { productId, name, description, features { ... } }
	    totalCount
	  }
	  
	  licenses(productId: "...", filter: {...}) {
	    items { licenseId, name, features { ... } }
	    totalCount
	  }
	  
	  features(productId: "...", optional: true) {
	    items { featureId, name, dependencies { ... } }
	    totalCount
	  }
	}
	```
	
	**Single Object Queries:**
	```graphql
	query {
	  product(id: "product-123") {
	    productId, name, description
	    features { featureId, name }
	    licenses { licenseId, name }
	  }
	  
	  feature(id: "feature-456") {
	    featureId, name, optional, permission
	    dependencies { featureId, name }
	  }
	  
	  license(id: "license-789") {
	    licenseId, name
	    features { featureId, name }
	    product { productId, name }
	  }
	}
	```
	
	**Dependency Queries:**
	```graphql
	query {
	  featureDependencies(featureId: "feature-456") {
	    directDependencies { featureId, name }
	    transitiveDependencies { featureId, name }
	    dependentFeatures { featureId, name }
	  }
	}
	```
	
	**Permission Queries:**
	```graphql
	query {
	  permissions(productId: "product-123") {
	    featureId
	    isPermission
	    isEnabled
	  }
	}
	```
	
	\subsection gql_mutations Mutations
	
	**Create Operations:**
	```graphql
	mutation {
	  createProduct(input: {
	    name: "New Product"
	    description: "Product description"
	    features: [...]
	  }) {
	    productId
	    name
	  }
	  
	  createLicense(input: {
	    productId: "product-123"
	    name: "Professional License"
	    features: ["feature-1", "feature-2"]
	  }) {
	    licenseId
	    name
	  }
	  
	  createFeature(input: {
	    name: "Advanced Analytics"
	    optional: true
	    permission: false
	  }) {
	    featureId
	    name
	  }
	}
	```
	
	**Update Operations:**
	```graphql
	mutation {
	  updateProduct(id: "product-123", input: {
	    name: "Updated Name"
	    features: [...]
	  }) {
	    productId
	    name
	  }
	  
	  updateLicense(id: "license-789", input: {
	    name: "Updated License"
	    features: ["feature-1", "feature-3"]
	  }) {
	    licenseId
	    name
	  }
	}
	```
	
	**Delete Operations:**
	```graphql
	mutation {
	  deleteProduct(id: "product-123") {
	    success
	    message
	  }
	  
	  deleteLicense(id: "license-789") {
	    success
	    message
	  }
	}
	```
	
	**Import/Export Operations:**
	```graphql
	mutation {
	  importProduct(file: $productFile) {
	    productId
	    name
	  }
	  
	  exportProduct(id: "product-123") {
	    fileContent
	    mimeType
	  }
	}
	```
	
	\subsection gql_subscriptions Subscriptions
	
	If supported by the imtservergql framework:
	```graphql
	subscription {
	  productUpdated(productId: "product-123") {
	    productId
	    name
	    updatedAt
	  }
	  
	  licenseActivated(productInstanceId: "instance-456") {
	    licenseInstanceId
	    expirationDate
	  }
	}
	```
	
	\section data_flow Data Flow Examples
	
	\subsection flow_create_product Creating a Product via GraphQL
	
	```
	1. GraphQL Request (POST /graphql)
	   └─> imtservergql: Request parsing
	       └─> imtlicgql: CProductCollectionControllerComp
	           ├─> Parse GraphQL input (product name, features, etc.)
	           ├─> Create IProductInfo object via factory
	           ├─> Create IFeatureInfo objects via factory
	           ├─> Validate product structure
	           └─> imtlic: CProductControllerComp
	               └─> imtlicdb: CProductsDatabaseDelegateComp
	                   └─> PostgreSQL: INSERT product JSON document
	                   <── Product ID
	               <── IProductInfo object
	           <── GraphQL response data
	       <── JSON response
	   <── HTTP 200 OK with product data
	```
	
	\subsection flow_query_features Querying Features via GraphQL
	
	```
	1. GraphQL Query (POST /graphql)
	   └─> imtservergql: Query parsing
	       └─> imtlicgql: CFeatureCollectionControllerComp
	           ├─> Parse query parameters (filters, pagination)
	           └─> imtlic: CFeatureInfoProviderComp
	               └─> imtlicdb: CFeatureDatabaseDelegateComp
	                   └─> PostgreSQL: SELECT features WHERE ...
	                   <── Feature JSON documents
	               <── IFeatureInfo objects
	           ├─> Convert to GraphQL representation
	           └─> Apply pagination and filtering
	           <── GraphQL response data
	       <── JSON response
	   <── HTTP 200 OK with feature list
	```
	
	\subsection flow_permission_check Permission Check via GraphQL
	
	```
	1. GraphQL Query (POST /graphql)
	   └─> imtservergql: Query parsing
	       └─> imtlicgql: CPermissionsProviderComp
	           └─> imtlic: CLicenseBasedRightsProviderComp
	               ├─> Query active licenses
	               ├─> Collect unlocked features
	               ├─> Check feature dependencies
	               └─> Return permission status
	               <── Permission data
	           <── GraphQL permission response
	       <── JSON response
	   <── HTTP 200 OK with permissions
	```
	
	\section integration Integration with Other Modules
	
	\subsection integration_imtlic Integration with imtlic (Licensing Core)
	
	**Consumes imtlic Types:**
	- IProductInfo, CProductInfo
	- IFeatureInfo, CFeatureInfo
	- ILicenseDefinition, CLicenseDefinition
	- IProductLicensingInfo
	
	**Uses imtlic Factories:**
	- Product factories for object creation
	- Feature factories for feature creation
	- License factories for license creation
	
	**Implements imtlic Interfaces:**
	- IFeatureDependenciesProvider (via CFeatureDependenciesProviderComp)
	
	**Delegates to imtlic Components:**
	- CProductControllerComp for product management
	- CLicenseControllerComp for license validation
	- CFeatureInfoProviderComp for feature access
	
	\subsection integration_imtservergql Integration with imtservergql (GraphQL Framework)
	
	**Extends Base Classes:**
	- Collection controllers extend CSdlCollectionControllerComp
	- Object controllers extend base controller classes
	- Uses SDL (Schema Definition Language) for type definitions
	
	**Uses GraphQL Infrastructure:**
	- Request/response handling
	- Schema validation
	- Query/mutation execution
	- Error handling
	
	**Leverages Features:**
	- Authentication/authorization hooks
	- Subscription support (if available)
	- File upload/download handling
	- WebSocket connections (for subscriptions)
	
	\subsection integration_imtlicdb Integration with imtlicdb (Database Layer)
	
	**Indirect Integration:**
	- imtlicgql calls imtlic components
	- imtlic components delegate to imtlicdb
	- No direct imtlicdb references in imtlicgql
	
	**Data Flow:**
	```
	imtlicgql (GraphQL) → imtlic (Domain) → imtlicdb (Persistence) → PostgreSQL
	```
	
	\subsection integration_clients Integration with Client Applications
	
	**Web Applications:**
	- JavaScript/TypeScript clients with GraphQL libraries (Apollo, Relay)
	- React, Vue, Angular frameworks
	- Real-time updates via subscriptions
	
	**Mobile Applications:**
	- iOS/Android with GraphQL clients
	- Offline support with local caching
	- Background synchronization
	
	**Desktop Applications:**
	- Qt-based applications with GraphQL queries
	- Direct HTTP requests to GraphQL endpoint
	- Integration with C++ client libraries
	
	**Development Tools:**
	- GraphQL Playground for API exploration
	- Schema introspection for documentation
	- Query testing and debugging
	
	\section best_practices Best Practices
	
	\subsection best_error_handling Error Handling
	- Return GraphQL errors for validation failures
	- Include error codes and messages
	- Log errors for debugging
	- Provide meaningful error descriptions
	
	\subsection best_performance Performance Optimization
	- Use pagination for large collections
	- Implement field-level resolvers
	- Cache frequently accessed data
	- Optimize database queries
	- Batch related queries
	
	\subsection best_security Security Considerations
	- Validate all input data
	- Check permissions before operations
	- Sanitize user input
	- Use authentication tokens
	- Implement rate limiting
	- Audit sensitive operations
	
	\subsection best_versioning API Versioning
	- Use GraphQL schema evolution
	- Deprecate fields instead of removing
	- Add new fields without breaking changes
	- Document breaking changes
	- Maintain backward compatibility
	
	\section documentation Additional Documentation
	
	- **LICENSING_ARCHITECTURE.md**: Complete three-module architecture documentation
	- **imtlic module documentation**: Core domain model details
	- **GraphQL Schema**: SDL files defining API structure
	- **API Examples**: Sample queries and mutations
	
	\see imtlic For core licensing domain model
	\see imtlicdb For database persistence layer
	\see imtservergql For GraphQL framework infrastructure
	\ingroup LicenseManagement
*/
namespace imtlicgql
{


} // namespace imtlicgql


