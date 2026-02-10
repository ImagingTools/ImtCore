// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


#include <imtlic/ILicenseDependenciesProvider.h>


namespace imtlic
{


/**
	Common interface for managing of dependencies for a license.
*/
class ILicenseDependenciesManager: virtual public ILicenseDependenciesProvider
{
public:
	/**
		Set list of licenses the given license depends on.
	*/
	virtual void SetLicenseDependencies(const QByteArray& licenseId, const QByteArrayList& dependentIds) = 0;
};


} // namespace imtlic


