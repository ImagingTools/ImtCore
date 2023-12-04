#include<imtupdate/CInstallationInfo.h>


// Acf includes
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>


namespace imtupdate
{


// public methods

void CInstallationInfo::SetProductVersion(const CInstallationVersionInfo& versionInfo)
{
	istd::CChangeNotifier changeNotifier(this);

	m_versionInfo = versionInfo;
}


void CInstallationInfo::SetReleaseDate(const QDateTime& releaseDate)
{
	if (releaseDate != m_releaseDate){
		istd::CChangeNotifier changeNotifier(this);

		m_releaseDate = releaseDate;
	}
}


void CInstallationInfo::SetChangeNotes(const QString& changeNotes)
{
	if (changeNotes != m_changeNotes){
		istd::CChangeNotifier changeNotifier(this);

		m_changeNotes = changeNotes;
	}
}


// reimplemented (istd::IChangable)

bool CInstallationInfo::ResetData(CompatibilityMode mode)
{
	istd::CChangeNotifier changeNotifier(this);

	m_versionInfo.ResetData(mode);
	m_releaseDate = QDateTime();
	m_changeNotes = QString();
	m_releaseDownloadUrl = QString();

	return true;
}


// reimplemented (iser::ISerializable)

bool CInstallationInfo::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier changeNotifier(archive.IsStoring() ? nullptr : this);

	bool retVal = true;

	static iser::CArchiveTag changeNotesTag("ChangeNotes", "Change notes for the product release");
	retVal = retVal && archive.BeginTag(changeNotesTag);
	retVal = retVal && archive.Process(m_changeNotes);
	retVal = retVal && archive.EndTag(changeNotesTag);

	static iser::CArchiveTag releaseDownloadUrlTag("DownloadUrl", "Url for downlading the installtion file");
	retVal = retVal && archive.BeginTag(releaseDownloadUrlTag);
	retVal = retVal && archive.Process(m_releaseDownloadUrl);
	retVal = retVal && archive.EndTag(releaseDownloadUrlTag);

	return retVal;
}


// reimplemented (IProductReleaseInfo)

const iser::IVersionInfo& CInstallationInfo::GetProductVersion() const
{
	return m_versionInfo;
}


QDateTime CInstallationInfo::GetReleaseDate() const
{
	return m_releaseDate;
}


QString CInstallationInfo::GetChangeNotes() const
{
	return m_changeNotes;
}


// reimplemented (IInstallationInfo)

QString CInstallationInfo::GetReleaseDownloadLink() const
{
	return m_releaseDownloadUrl;
}

QString CInstallationInfo::GetInstallationHashType() const
{
	return m_installationHashType;
}

QString CInstallationInfo::GetInstallationHashValue() const
{
	return m_installationHashValue;
}


void CInstallationInfo::SetReleaseDownloadLink(const QString& link)
{
	if (link != m_releaseDownloadUrl){
		istd::CChangeNotifier changeNotifier(this);

		m_releaseDownloadUrl = link;
	}
}

void CInstallationInfo::SetInstallationHashType(const QString& type)
{
	if (m_installationHashType != type){
		istd::CChangeNotifier changeNotifier(this);

		m_installationHashType = type;
	}

}

void CInstallationInfo::SetInstallationHashValue(const QString& value)
{
	if (m_installationHashValue != value){
		istd::CChangeNotifier changeNotifier(this);

		m_installationHashValue = value;
	}

}

} // namespace imtupdate

