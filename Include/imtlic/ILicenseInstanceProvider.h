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


class ILicenseInstance;


/**
	Interface for a provider of license instances.
	
	This interface is implemented by components that manage activated licenses,
	such as IProductInstanceInfo. It provides access to the collection of
	License Instances that are currently activated.
	
	\sa ILicenseInstance, IProductInstanceInfo
	\ingroup LicenseManagement
*/
class ILicenseInstanceProvider: virtual public istd::IChangeable
{
public:
	/**
		Get information about the collection of activated license instances.
		\return Collection info describing the activated licenses
	*/
	virtual const imtbase::ICollectionInfo& GetLicenseInstances() const = 0;

	/**
		Get a specific activated license instance by its ID.
		\param licenseId Unique identifier of the license
		\return Pointer to the license instance, or nullptr if not found
	*/
	virtual const ILicenseInstance* GetLicenseInstance(const QByteArray& licenseId) const = 0;
};


} // namespace imtlic


