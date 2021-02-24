#pragma once


// Qt includes
#include <QtCore/QByteArray>
#include <QtCore/QString>
#include <QtCore/QDateTime>

// ACF includes
#include <iser/IObject.h>


namespace imtlic
{


/**
	Common information about a license.
	\ingroup LicenseManagement
*/
class ILicenseInfo: virtual public iser::IObject
{
public:
	typedef QByteArrayList FeatureIds;

	/**
		Get human-readable name of the license.
	*/
	virtual QString GetLicenseName() const = 0;

	/**
		Set name of the license.
	*/
	virtual void SetLicenseName(const QString& licenseName) = 0;

	/**
		Get the license-ID. ID is used to identify the license during the license check.
		\sa iauth::IRightsProvider
	*/
	virtual QByteArray GetLicenseId() const = 0;
	
	/**
		Set unique ID of the license.
	*/
	virtual void SetLicenseId(const QByteArray& licenseId) = 0;

	/**
		Get expiration time for the license. An invalid time object means that the license has no time limitation.
	*/
	virtual QDateTime GetExpiration() const = 0;

	/**
		Set time of license expiration.
	*/
	virtual void SetExpiration(const QDateTime& expirationTime) = 0;

	/**
		Get the list of feature-IDs supported by this license.
	*/
	virtual FeatureIds GetFeatures() const = 0;

	/**
		Set the list of feature-IDs supported by this license.
	*/
	virtual void SetFeatures(const FeatureIds& featureIds) = 0;
};


} // namespace imtlic


