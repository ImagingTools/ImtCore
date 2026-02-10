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
	Common interface for a provider of licenses.
	\ingroup LicenseManagement
*/
class ILicenseInfoProvider: virtual public istd::IChangeable
{
public:
	/**
		Get information about the list of available licenses.
	*/
	virtual const imtbase::ICollectionInfo& GetLicenseList() const = 0;

	/**
		Get license info for the given ID.
	*/
	virtual istd::TUniqueInterfacePtr<ILicenseDefinition> GetLicenseInfo(const QByteArray& licenseId) const = 0;
};


} // namespace imtlic


