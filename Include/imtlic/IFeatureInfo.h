// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QSet>

// ACF includes
#include <idoc/IDocumentMetaInfo.h>
#include <iser/IObject.h>


namespace imtlic
{


/**
	\interface IFeatureInfo
	\brief Interface for accessing information about a feature within a product.
	
	A Feature represents an individual capability or functionality within a product.
	Features form the atomic units that define what a product can do. They can be
	organized hierarchically and can have dependencies on other features.
	
	Features are "unlocked" through License Definitions - each license specifies
	which features it enables when activated.
	
	\section feature_concept Conceptual Model
	
	**Features as Capabilities:**
	- Atomic unit of product functionality
	- Can be mandatory (always included) or optional (sold separately)
	- Can be permission-based (access control) vs. feature-based (capability)
	- Organized in hierarchies for complex products
	
	**Feature Hierarchy Example:**
	```
	Product "Office Suite"
	├─ Feature "Document Editing" (Mandatory)
	│  ├─ Feature "Basic Formatting" (Mandatory)
	│  ├─ Feature "Advanced Formatting" (Optional)
	│  └─ Feature "Track Changes" (Optional)
	├─ Feature "Spreadsheet" (Optional)
	│  ├─ Feature "Basic Functions" (Mandatory)
	│  └─ Feature "Pivot Tables" (Optional)
	└─ Feature "Presentations" (Optional)
	```
	
	\section feature_types Feature Types
	
	**Optional vs. Mandatory:**
	- **Optional Features**: Can be sold separately, customer chooses which to purchase
	- **Mandatory Features**: Always included, part of core product functionality
	- Example: Core editing (mandatory), advanced analytics (optional)
	
	**Permission vs. Capability:**
	- **Permission Features**: Access control rights (NOT sellable)
	  - Example: "Close Application", "Export Data", "Admin Access"
	  - Control what users can do with the application
	  - Managed through license-based rights system
	
	- **Capability Features**: Product functionality (sellable)
	  - Example: "Advanced Analytics", "PDF Export", "Cloud Sync"
	  - Actual features that customers purchase
	  - Enabled through license activation
	
	\section feature_dependencies Feature Dependencies
	
	Features can depend on other features:
	```
	Feature "Pivot Tables"
	├─ Depends on: "Basic Spreadsheet Functions"
	└─ Depends on: "Data Import"
	```
	
	When checking if a feature is available:
	1. Check if feature is unlocked by active license
	2. Check if all dependent features are also unlocked
	3. Recursively validate transitive dependencies
	4. Feature available only if all dependencies satisfied
	
	\section feature_hierarchy Feature Hierarchies
	
	Features support parent-child relationships:
	- **Parent Feature**: Container of related sub-features
	- **Child Features**: Specialized capabilities within parent
	- **Depth Control**: Can traverse hierarchy to specific depth
	- **Navigation**: Can access parent or children
	
	Benefits of hierarchies:
	- Logical grouping of related features
	- Easier product management
	- Clearer customer communication
	- Simplified license definitions
	
	\section feature_metainfo Meta-Information
	
	Features support meta-information for persistence:
	- MIT_FEATURE_ID: QByteArray - Unique feature identifier
	- MIT_FEATURE_NAME: QString - Human-readable feature name
	- MIT_FEATURE_DESCRIPTION: QString - Detailed description
	- MIT_IS_OPTIONAL: bool - Whether feature is optional
	- MIT_IS_PERMISSION: bool - Whether feature is a permission
	
	\section feature_usage Usage Examples
	
	**Creating a Feature:**
	```cpp
	IFeatureInfoSharedPtr feature = ...;  // From factory
	feature->SetFeatureId("feature-advanced-analytics");
	feature->SetFeatureName("Advanced Analytics");
	feature->SetFeatureDescription("Data analysis and reporting tools");
	feature->SetIsOptional(true);  // Can be sold separately
	feature->SetIsPermission(false);  // This is a capability, not permission
	```
	
	**Building Feature Hierarchy:**
	```cpp
	IFeatureInfoSharedPtr parent = ...; // "Document Editing"
	IFeatureInfoSharedPtr child1 = ...; // "Basic Formatting"
	IFeatureInfoSharedPtr child2 = ...; // "Advanced Formatting"
	
	parent->InsertSubFeature(child1);
	parent->InsertSubFeature(child2);
	
	// Navigate hierarchy
	const FeatureInfoList& children = parent->GetSubFeatures();
	const IFeatureInfo* parentRef = child1->GetParentFeature();
	```
	
	**Setting Dependencies:**
	```cpp
	IFeatureInfoSharedPtr pivotTables = ...;
	IFeatureInfoSharedPtr basicSpreadsheet = ...;
	
	FeatureIds dependencies;
	dependencies.insert(basicSpreadsheet->GetFeatureId());
	pivotTables->SetDependsOnFeatureIds(dependencies);
	
	// Check dependencies
	FeatureIds deps = pivotTables->GetDependsOnFeatureIds();
	for (const QByteArray& depId : deps) {
	    // Validate dependency is available
	}
	```
	
	\section feature_licensing Feature Licensing
	
	Features are unlocked through licenses:
	1. License Definition specifies which features it includes
	2. Customer purchases and activates license
	3. License Instance created on Product Instance
	4. Features from all active licenses become available
	5. Application checks feature availability before use
	
	**Example License Configuration:**
	```
	Basic License:    [Core Editing, Basic Formatting]
	Pro License:      [Core Editing, Basic Formatting, Advanced Formatting, Spreadsheet]
	Enterprise:       [All Features]
	```
	
	\section feature_relationships Related Interfaces
	
	- **IProductInfo**: Product containing this feature
	- **ILicenseDefinition**: License that unlocks this feature
	- **IFeatureInfoProvider**: Access to feature catalog
	- **IFeatureDependenciesProvider**: Dependency resolution
	- **CLicenseBasedRightsProviderComp**: Permission provider based on feature/permission availability (implements IRightsProvider)
	
	\sa IProductInfo, ILicenseDefinition, IFeatureInfoProvider, IFeatureDependenciesProvider
	\ingroup LicenseManagement
*/
class IFeatureInfo: virtual public iser::IObject
{
public:
	enum MetaInfoTypes
	{
		MIT_FEATURE_ID = idoc::IDocumentMetaInfo::MIT_USER + 1,
		MIT_FEATURE_NAME,
		MIT_FEATURE_DESCRIPTION,
		MIT_IS_OPTIONAL,
		MIT_IS_PERMISSION
	};

	typedef QSet<QByteArray> FeatureIds;
	typedef istd::TSharedInterfacePtr<IFeatureInfo> FeatureInfoPtr;
	typedef QVector<FeatureInfoPtr> FeatureInfoList;

	/**
		Get the unique feature identifier.
		\return Unique ID of this feature
	*/
	virtual QByteArray GetFeatureId() const = 0;

	/**
		Get the human-readable feature name.
		\return Display name of this feature
	*/
	virtual QString GetFeatureName() const = 0;

	/**
		Get the detailed feature description.
		\return Description explaining what this feature provides
	*/
	virtual QString GetFeatureDescription() const = 0;

	/**
		Check if this feature is optional.
		
		**Important:** Optional features represent functionality that can be sold optionally.
		They are NOT mandatory for the core functionality of the product and can be
		purchased separately by customers based on their needs. This allows flexible
		product configurations where customers only pay for the features they require.
		
		\return true if the feature is optional (can be sold separately), false if mandatory (part of core product)
	*/
	virtual bool IsOptional() const = 0;

	/**
		Check if this feature represents a permission.
		
		**Important:** Permission features are NOT sellable features. They represent access
		control rights or operational permissions within the application, not product capabilities.
		Permissions control what users can do with the application itself.
		
		**Example:** A "Close" permission would control whether the application can be exited,
		which is an operational right, not a product feature that can be sold.
		
		\return true if this is a permission feature (access control), false if it's a sellable feature
	*/
	virtual bool IsPermission() const = 0;

	/**
		Get the parent feature in the feature hierarchy.
		Features can form tree structures with parent-child relationships.
		\return Pointer to parent feature, or nullptr if this is a root feature
	*/
	virtual const IFeatureInfo* GetParentFeature() const = 0;

	/**
		Get IDs of all sub-features (children) in the hierarchy.
		\param maxDepth Maximum traversal depth (-1 for unlimited depth)
		\return List of sub-feature IDs
	*/
	virtual QByteArrayList GetSubFeatureIds(int maxDepth = -1) const = 0;

	/**
		Get a specific sub-feature by ID.
		\param subfeatureId ID of the sub-feature to find
		\param maxDepth Maximum traversal depth for the search (-1 for unlimited)
		\return Pointer to the sub-feature, or nullptr if not found
	*/
	virtual FeatureInfoPtr GetSubFeature(const QByteArray& subfeatureId, int maxDepth = -1) const = 0;

	/**
		Get the direct child features of this feature.
		\return List of immediate sub-features
	*/
	virtual const FeatureInfoList& GetSubFeatures() const  = 0;

	/**
		Add a sub-feature as a child of this feature.
		\param subFeatureInfo Feature to be added as a child
		\return true if successfully added, false otherwise
	*/
	virtual bool InsertSubFeature(FeatureInfoPtr subFeatureInfo) = 0;

	/**
		Remove a sub-feature from this feature's children.
		\param subFeatureId ID of the sub-feature to remove
	*/
	virtual void DeleteSubFeature(const QByteArray& subFeatureId) = 0;

	/**
		Get the list of features that this feature depends on.
		These dependencies must be satisfied for this feature to function properly.
		\return List of feature IDs that this feature depends on
	*/
	virtual QByteArrayList GetDependencies() const = 0;
};


typedef istd::TUniqueInterfacePtr<IFeatureInfo> IFeatureInfoUniquePtr;
typedef istd::TSharedInterfacePtr<IFeatureInfo> IFeatureInfoSharedPtr;


} // namespace imtlic


