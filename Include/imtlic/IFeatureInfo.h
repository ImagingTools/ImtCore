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
	Interface for accessing information about a feature.
	
	A Feature represents an individual capability or functionality within a product.
	Features form the atomic units that define what a product can do. They can be
	organized hierarchically and can have dependencies on other features.
	
	Features are "unlocked" through License Definitions - each license specifies
	which features it enables when activated.
	
	Key characteristics:
	- Can be optional (not required) or mandatory
	- Can form parent-child hierarchies for complex feature trees
	- Can have dependencies on other features
	- Can be marked as permissions for access control purposes
	
	\sa IProductInfo, ILicenseDefinition
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
		Optional features are not required for basic product operation.
		\return true if the feature is optional, false if mandatory
	*/
	virtual bool IsOptional() const = 0;

	/**
		Check if this feature represents a permission.
		Permission features are typically used for access control.
		\return true if this is a permission feature
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


