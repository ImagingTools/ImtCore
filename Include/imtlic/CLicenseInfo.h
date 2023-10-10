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

	CLicenseInfo();

	// reimplemented (imtlic::ILicenseInfo)
	virtual QString GetLicenseName() const override;
	void SetLicenseName(const QString& licenseName) override;
	virtual QByteArray GetLicenseId() const override;
	void SetLicenseId(const QByteArray& licenseId) override;
	virtual QString GetLicenseDescription() const override;
	virtual void SetLicenseDescription(const QString& licenseDescription) override;
	virtual QByteArray GetProductId() const override;
	virtual void SetProductId(const QByteArray& productId) override;
	virtual QByteArrayList GetFeatures() const override;
	virtual void SetFeatures(QByteArrayList features) override;

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
	QByteArray m_licenseId;
	QByteArray m_productId;
	QString m_licenseName;
	QString m_description;
	QByteArrayList m_featureIds;
};


} // namespace imtlic


