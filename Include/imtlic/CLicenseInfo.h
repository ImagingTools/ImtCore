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
	void SetLicenseName(const QString& licenseName);
	void SetLicenseId(const QByteArray& licenseId);
	void SetPackageId(const QByteArray& packageId);
	void SetExpiration(const QDateTime& expirationTime);

	// reimplemented (imtlic::ILicenseInfo)
	virtual QString GetLicenseName() const override;
	virtual QByteArray GetLicenseId() const override;
	virtual QByteArray GetPackageId() const override;
	virtual QDateTime GetExpiration() const override;

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
	QByteArray m_packageId;
	QDateTime m_expirationTime;
};


} // namespace imtlic


