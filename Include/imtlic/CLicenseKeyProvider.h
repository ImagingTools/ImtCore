// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtlic/IEncyptedLicenseKeyProvider.h>


namespace imtlic
{


/**
	Implementation of access to an encrypted license key based on the cryptographic hash.
	\ingroup LicenseManagement
*/
class CLicenseKeyProvider: virtual public imtlic::IEncyptedLicenseKeyProvider
{
public:
	// reimplemented (imtlic::IEncyptedLicenseKeyProvider)
	virtual QByteArray GetLicenseKey(
				const IProductInstanceInfo& instance,
				const QByteArray& licenseId,
				const QByteArray& privateKey) const override;
};


} // namespace imtlic


