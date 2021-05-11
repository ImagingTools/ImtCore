#pragma once


// ImtCore includes
#include <imtbase/IObjectCollection.h>
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

	CLicenseInfo();

	// reimplemented (imtlic::ILicenseInfo)
	virtual const imtbase::IObjectCollection* GetFeaturePackages() const override;
	virtual QString GetLicenseName() const override;
	void SetLicenseName(const QString& licenseName) override;
	virtual QByteArray GetLicenseId() const override;
	void SetLicenseId(const QByteArray& licenseId) override;
	virtual FeatureInfos GetFeatureInfos() const override;
	virtual void SetFeatureInfos(const FeatureInfos& featureInfos) override;

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
	const imtbase::IObjectCollection* m_featurePackageCollectionPtr;
	QString m_licenseName;
	QByteArray m_licenseId;
	FeatureInfos m_featureInfos;
};


} // namespace imtlic


