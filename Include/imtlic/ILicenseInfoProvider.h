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


class ILicenseDefinition;


/**
	Interface for a provider of license definitions.
	
	This interface is implemented by components that provide access to
	License Definition catalogs, such as IProductLicensingInfo.
	
	\sa ILicenseDefinition, IProductLicensingInfo
	\ingroup LicenseManagement
*/
class ILicenseInfoProvider: virtual public istd::IChangeable
{
public:
	/**
		Get information about the collection of available license definitions.
		\return Collection info describing the available licenses
	*/
	virtual const imtbase::ICollectionInfo& GetLicenseList() const = 0;

	/**
		Get a specific license definition by its ID.
		\param licenseId Unique identifier of the license
		\return Pointer to the license definition, or nullptr if not found
	*/
	virtual istd::TUniqueInterfacePtr<ILicenseDefinition> GetLicenseInfo(const QByteArray& licenseId) const = 0;
};


} // namespace imtlic


