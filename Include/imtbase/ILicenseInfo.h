#pragma once


// Qt includes
#include <QtCore/QByteArray>
#include <QtCore/QString>
#include <QtCore/QDateTime>

// ACF includes
#include <iser/IObject.h>


namespace imtbase
{


/**
	Common information about a license.
	\ingroup LicenseManagement
*/
class ILicenseInfo: virtual public iser::IObject
{
public:
	/**
		Get human-readable name of the license.
	*/
	virtual QString GetLicenseName() const = 0;

	/**
		Get the license-ID. ID is used to identify the license during the license check.
		\sa iauth::IRightsProvider
	*/
	virtual QByteArray GetLicenseId() const = 0;

	/**
		Get ID of the package containing this license if available.
	*/
	virtual QByteArray GetPackageId() const = 0;

	/**
		Get expiration time for the license. An invalid time object means that the license has no time limitation.
	*/
	virtual QDateTime GetExpiration() const = 0;
};


} // namespace imtbase


