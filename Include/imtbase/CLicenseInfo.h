#pragma once


// Qt includes
#include <QtCore/QByteArray>
#include <QtCore/QString>
#include <QtCore/QDateTime>

// ACF includes
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>

// ImtCore includes
#include <imtbase/ILicenseInfo.h>


namespace imtbase
{


/**
	Common status manager implementation.
*/
class CLicenseInfo: virtual public imtbase::ILicenseInfo
{
public:
	void SetLicenseName(const QString& licenseName);
	void SetLicenseId(const QByteArray& licenseId);
	void SetPackageId(const QByteArray& packageId);
	void SetExpiration(const QDateTime& expirationTime);

	// reimplemented (imtbase::ILicenseInfo)
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


} // namespace imtbase


