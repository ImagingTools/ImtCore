#pragma once


// ImtCore includes
#include <imtlic/IEncyptedLicenseKeyProvider.h>


namespace imtlic
{


/**
	Implementation of access to an encrypted license key based on cryptographic hash.
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


