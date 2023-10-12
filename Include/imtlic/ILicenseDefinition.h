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
class ILicenseDefinition: virtual public iser::IObject
{
public:
	struct FeatureInfo
	{
		QByteArray id;
		QString name;

		bool operator==(const FeatureInfo& other) const
		{
			return (id == other.id) && (name == other.name);
		}

		bool operator!=(const FeatureInfo& other) const
		{
			return !operator==(other);
		}
	};

	typedef QVector<FeatureInfo> FeatureInfos;

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
		Get description of the license.
	*/
	virtual QString GetLicenseDescription() const = 0;

	/**
		Set description of the license.
	*/
	virtual void SetLicenseDescription(const QString& licenseDescription) = 0;

	/**
		Get the product ID of the license.
	*/
	virtual QByteArray GetProductId() const = 0;

	/**
		Set product ID of the license.
	*/
	virtual void SetProductId(const QByteArray& productId) = 0;

//	/**
//		Get the list of feature-IDs supported by this license.
//	*/
//	virtual QByteArrayList GetFeatures() const = 0;

//	/**
//		Set the list of feature-IDs supported by this license.
//	*/
//	virtual void SetFeatures(QByteArrayList features) = 0;

	/**
		Get list of licenses the given license depends on.
	*/
	virtual QByteArrayList GetDependencies() const = 0;

	/**
		Set dependencies.
	*/
	virtual void SetDependencies(QByteArrayList dependencies) = 0;

	/**
		Get the list of feature-IDs supported by this license.
	*/
	virtual ILicenseDefinition::FeatureInfos GetFeatureInfos() const = 0;

	/**
		Set the list of feature-IDs supported by this license.
	*/
	virtual void SetFeatureInfos(const ILicenseDefinition::FeatureInfos& featureInfos) = 0;
};


} // namespace imtlic


