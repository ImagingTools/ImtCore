#pragma once


// ImtCore includes
#include <imtlic/ILicenseInfo.h>


namespace imtlic
{


/**
	Common implementation of the information about a single license.
	\sa ILicenseInfo
	\ingroup LicenseManagement
*/
class CLicenseInfo: virtual public imtlic::ILicenseInfo
{
public:
	static QByteArray GetTypeId();

	// reimplemented (imtlic::ILicenseInfo)
	virtual QString GetLicenseName() const override;
	void SetLicenseName(const QString& licenseName) override;
	virtual QByteArray GetLicenseId() const override;
	void SetLicenseId(const QByteArray& licenseId) override;
	virtual QDateTime GetExpiration() const override;
	void SetExpiration(const QDateTime& expirationTime) override;
	virtual FeatureIds GetFeatures() const override;
	virtual void SetFeatures(const FeatureIds& featureIds) override;

	// reimplemented (iser::IObject)
	virtual QByteArray GetFactoryId() const override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual IChangeable* CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

protected:
	QString m_licenseName;
	QByteArray m_licenseId;
	QDateTime m_expirationTime;
	FeatureIds m_featureIds;
};


} // namespace imtlic


