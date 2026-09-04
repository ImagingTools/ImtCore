// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QHash>
#include <QtCore/QSet>
#include <QtCore/QVector>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtlic/IFeatureInfo.h>
#include <imtlic/IProductInfo.h>


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
	       └─> imtauthgql: CPermissionsProviderComp
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


/**
	What a product holds once the requirements of its features have been followed.

	\see CompleteProductFeatures
*/
struct ProductFeatureClosure
{
	/**
		Feature document ids: the ones the product was given, in that order,
		followed by the ones its requirements pulled in.
	*/
	QByteArrayList featureIds;

	/**
		Optional parts the product takes, including the ones a requirement points at.
	*/
	imtlic::IProductInfo::OptionalFeatureInfos optionalFeatures;

	/**
		Requirement paths naming a feature the catalog does not (or no longer) have.
		A product is still completed around them; they are reported so the caller
		can say the catalog is inconsistent.
	*/
	QByteArrayList unresolvedRequirements;
};


namespace Detail
{


/**
	One node of the feature catalog, as the requirement resolution needs it.
	A node is either a feature itself (empty subFeatureId) or a part inside one.
*/
struct FeatureCatalogNode
{
	QByteArray featureId;
	QByteArray subFeatureId;
	bool isOptional = false;
	QByteArrayList requirements;
};


inline void CollectCatalogNodes(
			const imtlic::IFeatureInfo& featureInfo,
			const QByteArray& featureId,
			const QByteArray& parentPath,
			bool isRoot,
			QHash<QByteArray, FeatureCatalogNode>& nodesByPath,
			QHash<QByteArray, QVector<FeatureCatalogNode>>& nodesByFeatureId)
{
	const QByteArray nodeId = featureInfo.GetFeatureId();
	if (nodeId.isEmpty()){
		return;
	}

	// The form requirements are written in (\see imtlic::CalculateFeaturePath):
	// the ids of the feature and of every part above this node.
	const QByteArray nodePath = parentPath + '/' + nodeId;

	FeatureCatalogNode node;
	node.featureId = featureId;
	node.subFeatureId = isRoot ? QByteArray() : nodeId;
	node.isOptional = featureInfo.IsOptional();
	node.requirements = featureInfo.GetRequirements();

	nodesByPath.insert(nodePath, node);
	nodesByFeatureId[featureId].append(node);

	const imtlic::IFeatureInfo::FeatureInfoList& subFeatures = featureInfo.GetSubFeatures();
	for (const imtlic::IFeatureInfo::FeatureInfoPtr& subFeaturePtr : subFeatures){
		if (subFeaturePtr.IsValid()){
			CollectCatalogNodes(*subFeaturePtr, featureId, nodePath, false, nodesByPath, nodesByFeatureId);
		}
	}
}


} // namespace Detail


/**
	Complete the features of a product with everything they require.

	A feature declares what it requires as full feature paths, and a product that
	takes the feature has to take those as well - transitively, since a required
	feature requires in turn. This resolves those paths against the feature
	catalog and answers with what the product must hold:

	- a requirement naming a feature adds that feature to the product;
	- a requirement naming a mandatory part of a feature adds the feature it
	  belongs to, because a mandatory part comes with it;
	- a requirement naming an optional part adds the feature and marks that part
	  as taken.

	Only the parts a product actually has speak: the requirements of an optional
	part are followed once the product takes it, never before.

	The result is what the product is to be filled with; nothing is removed from
	what was given. Which of the features came in this way is not recorded - it
	follows from the requirements again, so it cannot go stale (\see ProductView.qml,
	which marks those rows and refuses to drop them while their source stays).

	\param featureCollection The feature catalog: every feature a product may take.
	\param featureIds Feature document ids the product was given.
	\param optionalFeatures Optional parts the product was given, per feature.
*/
inline ProductFeatureClosure CompleteProductFeatures(
			const imtbase::IObjectCollection& featureCollection,
			const QByteArrayList& featureIds,
			const imtlic::IProductInfo::OptionalFeatureInfos& optionalFeatures)
{
	ProductFeatureClosure retVal;

	// The catalog, flattened once: reading an element hands out a deep copy of a
	// whole feature tree, so it is walked here and answered from the maps below.
	QHash<QByteArray, Detail::FeatureCatalogNode> nodesByPath;
	QHash<QByteArray, QVector<Detail::FeatureCatalogNode>> nodesByFeatureId;

	const imtbase::ICollectionInfo::Ids catalogIds = featureCollection.GetElementIds();
	for (const imtbase::ICollectionInfo::Id& catalogId : catalogIds){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (!featureCollection.GetObjectData(catalogId, dataPtr)){
			continue;
		}

		const imtlic::IFeatureInfo* featureInfoPtr = dynamic_cast<const imtlic::IFeatureInfo*>(dataPtr.GetPtr());
		if (featureInfoPtr != nullptr){
			Detail::CollectCatalogNodes(*featureInfoPtr, catalogId, QByteArray(), true, nodesByPath, nodesByFeatureId);
		}
	}

	QSet<QByteArray> knownFeatureIds;
	QByteArrayList pendingFeatureIds;
	for (const QByteArray& featureId : featureIds){
		if (!featureId.isEmpty() && !knownFeatureIds.contains(featureId)){
			knownFeatureIds.insert(featureId);
			retVal.featureIds.append(featureId);
			pendingFeatureIds.append(featureId);
		}
	}

	// Parts taken, by the feature they belong to. Entries naming a feature the
	// product does not hold are dropped: a part is only taken from a feature the
	// product has.
	QHash<QByteArray, QSet<QByteArray>> takenSubFeatureIds;
	for (const imtlic::IProductInfo::OptionalFeatureInfo& optionalFeature : optionalFeatures){
		if (!knownFeatureIds.contains(optionalFeature.featureId)){
			continue;
		}

		for (const QByteArray& subFeatureId : optionalFeature.subFeatureIds){
			if (!subFeatureId.isEmpty()){
				takenSubFeatureIds[optionalFeature.featureId].insert(subFeatureId);
			}
		}
	}

	QSet<QByteArray> reportedRequirements;

	while (!pendingFeatureIds.isEmpty()){
		const QByteArray featureId = pendingFeatureIds.takeFirst();

		const QVector<Detail::FeatureCatalogNode> nodes = nodesByFeatureId.value(featureId);
		for (const Detail::FeatureCatalogNode& node : nodes){
			// An optional part the product did not take is not part of it, so
			// what it requires is none of this product's business.
			if (!node.subFeatureId.isEmpty()
						&& node.isOptional
						&& !takenSubFeatureIds.value(featureId).contains(node.subFeatureId)){
				continue;
			}

			for (const QByteArray& requirement : node.requirements){
				if (requirement.isEmpty()){
					continue;
				}

				QHash<QByteArray, Detail::FeatureCatalogNode>::const_iterator requiredIter =
						nodesByPath.constFind(requirement);
				if (requiredIter == nodesByPath.constEnd()){
					if (!reportedRequirements.contains(requirement)){
						reportedRequirements.insert(requirement);
						retVal.unresolvedRequirements.append(requirement);
					}

					continue;
				}

				const Detail::FeatureCatalogNode& requiredNode = requiredIter.value();

				if (!knownFeatureIds.contains(requiredNode.featureId)){
					knownFeatureIds.insert(requiredNode.featureId);
					retVal.featureIds.append(requiredNode.featureId);
					pendingFeatureIds.append(requiredNode.featureId);
				}

				// A mandatory part needs nothing beyond its feature; an optional
				// one has to be taken, and what it requires then counts too.
				if (!requiredNode.subFeatureId.isEmpty()
							&& requiredNode.isOptional
							&& !takenSubFeatureIds.value(requiredNode.featureId).contains(requiredNode.subFeatureId)){
					takenSubFeatureIds[requiredNode.featureId].insert(requiredNode.subFeatureId);
					pendingFeatureIds.append(requiredNode.featureId);
				}
			}
		}
	}

	// Written back in the order of the features, so the same product always
	// yields the same document.
	for (const QByteArray& featureId : retVal.featureIds){
		const QSet<QByteArray> subFeatureIds = takenSubFeatureIds.value(featureId);
		if (subFeatureIds.isEmpty()){
			continue;
		}

		imtlic::IProductInfo::OptionalFeatureInfo optionalFeatureInfo;
		optionalFeatureInfo.featureId = featureId;
		optionalFeatureInfo.subFeatureIds = QByteArrayList(subFeatureIds.begin(), subFeatureIds.end());
		std::sort(optionalFeatureInfo.subFeatureIds.begin(), optionalFeatureInfo.subFeatureIds.end());

		retVal.optionalFeatures << optionalFeatureInfo;
	}

	return retVal;
}


} // namespace imtlicgql


