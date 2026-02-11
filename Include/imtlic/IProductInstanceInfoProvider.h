// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IChangeable.h>


namespace imtbase
{
	class ICollectionInfo;
}


namespace imtlic
{


class IProductInstanceInfo;


/**
	Interface for a provider of product instances.
	
	This interface is implemented by components that manage collections of
	Product Instances, providing access to all installed product instances
	in the system.
	
	\sa IProductInstanceInfo
	\ingroup LicenseManagement
*/
class IProductInstanceInfoProvider: virtual public istd::IChangeable
{
public:
	enum MetaInfoTypes
	{
		/**
			List of product instance information given as QStringList.
		*/
		MIT_LICENSE_INSTANCE_INFO_LIST = idoc::IDocumentMetaInfo::MIT_USER + 1
	};

	/**
		Get information about the collection of all product instances.
		\return Collection info describing all installed product instances
	*/
	virtual const imtbase::ICollectionInfo& GetProductInstanceList() const = 0;

	/**
		Get a specific product instance by its instance ID.
		\param instanceId Unique identifier of the product instance
		\return Pointer to the product instance, or nullptr if not found
	*/
	virtual const IProductInstanceInfo* GetProductInstance(const QByteArray& instanceId) const = 0;
};


} // namespace imtlic


