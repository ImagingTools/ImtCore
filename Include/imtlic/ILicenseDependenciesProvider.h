// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IChangeable.h>


namespace imtlic
{


/**
	Common interface for providing of dependencies for a license.
*/
class ILicenseDependenciesProvider: virtual public istd::IChangeable
{
public:
	/**
		Get list of licenses the given license depends on.
	*/
	virtual QByteArrayList GetLicenseDependencies(const QByteArray& licenseId) const = 0;
};


} // namespace imtlic


