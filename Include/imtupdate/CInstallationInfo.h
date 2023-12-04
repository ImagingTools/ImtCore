#pragma once

// Qt includes
#include <QtCore/QDateTime>

// imtupdate includes
#include<imtupdate/IInstallationInfo.h>
#include<imtupdate/CInstallationVersionInfo.h>


namespace imtupdate
{


class CInstallationInfo: public IInstallationInfo
{
public:
	void SetProductVersion(const CInstallationVersionInfo& versionInfo);
	void SetReleaseDate(const QDateTime& releaseDate);
	void SetChangeNotes(const QString& changeNotes);
	void SetReleaseDownloadLink(const QString& link);
	void SetInstallationHashType(const QString& type);
	void SetInstallationHashValue(const QString& value);

	// reimplemented (istd::IChangable)
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (IProductReleaseInfo)
	virtual const iser::IVersionInfo& GetProductVersion() const override;
	virtual QDateTime GetReleaseDate() const override;
	virtual QString GetChangeNotes() const override;

	// reimplemented (IInstallationInfo)
	virtual QString GetReleaseDownloadLink() const override;
	virtual QString GetInstallationHashType() const override;
	virtual QString GetInstallationHashValue() const override;

private:
	QString m_releaseDownloadUrl;
	QDateTime m_releaseDate;
	QString m_changeNotes;
	QString m_installationHashType;
	QString m_installationHashValue;

	CInstallationVersionInfo m_versionInfo;
};


} // namespace imtupdate

