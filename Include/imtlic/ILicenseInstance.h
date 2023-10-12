#pragma once


// ACF includes
#include <imtlic/ILicenseDefinition.h>



namespace imtlic
{


/**
	Common information about a license instance.
	\ingroup LicenseManagement
*/
class ILicenseInstance: virtual public ILicenseDefinition
{
public:
	/**
		Get expiration time for the license instance. An invalid time object means that the license has no time limitation.
	*/
	virtual QDateTime GetExpiration() const = 0;

	/**
		Set time of license expiration.
	*/
	virtual void SetExpiration(const QDateTime& expirationTime) = 0;
};


} // namespace imtlic


