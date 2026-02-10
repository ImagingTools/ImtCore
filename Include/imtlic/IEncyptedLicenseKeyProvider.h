// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IChangeable.h>


namespace imtlic
{


class IProductInstanceInfo;


/**
	Provider of encrypted license keys for a product instance.
	\ingroup LicenseManagement
*/
class IEncyptedLicenseKeyProvider: virtual public istd::IChangeable
{
public:
	/**
		Get the encrypted license key for a given license-ID.
	*/
	virtual QByteArray GetLicenseKey(
				const IProductInstanceInfo& instance,
				const QByteArray& licenseId,
				const QByteArray& privateKey) const = 0;
};


} // namespace imtlic


