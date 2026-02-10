// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <idoc/IDocumentMetaInfo.h>

// ImtCore includes
#include <imtlic/IFeatureInfo.h>


namespace imtbase
{
	class ICollectionInfo;
}


namespace imtlic
{


/**
	Interface for a provider of feature information.
	
	This interface is implemented by components that manage feature catalogs,
	such as product definitions. It provides access to feature definitions
	and their hierarchical relationships.
	
	\sa IFeatureInfo, IProductInfo
	\ingroup LicenseManagement
*/
class IFeatureInfoProvider: virtual public istd::IChangeable
{
public:
	enum MetaInfoTypes
	{
		/**
			List of feature information given as QStringList.
		*/
		MIT_FEATURE_INFO_LIST = idoc::IDocumentMetaInfo::MIT_USER + 1,
	};

	/**
		Get information about the collection of available features.
		\return Collection info describing the available features
	*/
	virtual const imtbase::ICollectionInfo& GetFeatureList() const = 0;

	/**
		Get a specific feature by its ID.
		\param featureId Unique identifier of the feature
		\return Pointer to the feature information, or nullptr if not found
	*/
	virtual IFeatureInfoSharedPtr GetFeatureInfo(const QByteArray& featureId) const = 0;
};


} // namespace imtlic


