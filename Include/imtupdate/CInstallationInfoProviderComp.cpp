#include<imtupdate/CInstallationInfoProviderComp.h>


// Qt includes
#include <QtCore/QTimer>
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QUuid>
#include <QtCore/QStandardPaths>
#include <QtCore/QUrl>

// ACF includes
#include <ifile/CCompactXmlFileReadArchive.h>

// imtupdate includes
#include<imtupdate/imtupdate.h>

namespace imtupdate
{


// public methods

void CInstallationInfoProviderComp::SetInstallationInfo(const CInstallationInfo& installationInfo)
{
	m_installationInfo = installationInfo;
}


// reimplemented (IInstallationInfoProvider)

const IInstallationInfo* CInstallationInfoProviderComp::GetInstallationInfo() const
{
	if(!AcquireInstallationInfo(*m_downloadLinkAttrPtr)){
		return nullptr;
	}

	return &m_installationInfo;
}


// protected methods

bool CInstallationInfoProviderComp::AcquireInstallationInfo(
			const QString& link,
			ibase::IProgressManager* progressManagerPtr) const
{
	m_installationInfo.ResetData();

#if defined (Q_OS_ANDROID)
	const char* const operationSystemPath = "/ANDROID";
#elif defined(Q_OS_WIN)
	const char* const operationSystemPath = "/WINDOWS";
#elif defined(Q_OS_MAC)
	const char* const operationSystemPath = "/OSX";
#elif defined(Q_OS_LINUX)
	const char* const operationSystemPath = "/LINUX";
#endif

	QByteArray downloadPrefix =  QByteArray("UPDATES/") + m_productNameAttrPtr->GetValue().toUtf8() + QByteArray(operationSystemPath);
	QByteArray downloadLink = downloadPrefix + "/MANIFEST";
	QFile manifestTempFile(GetPathToInstall() + '/' + QFileInfo(QCoreApplication::applicationFilePath()).baseName() + ".xmanifest");

	if(manifestTempFile.exists()){
		manifestTempFile.remove();
	}

	IDownloader::DownloadStatus downloadResult = IDownloader::DS_FAILED;
	downloadResult = m_downloaderCompPtr->DownloadFile(downloadLink, manifestTempFile.fileName(), false, progressManagerPtr);
	if (downloadResult != IDownloader::DS_OK){
		SendErrorMessage(0, "Unable to download manifest");

		return false;
	}


	ifile::CCompactXmlFileReadArchive xmlReadArchive(manifestTempFile.fileName(), false);
	CUpdateManifest manifest;
	bool isSerialized = manifest.Serialize(xmlReadArchive);
	if (!isSerialized){
		SendErrorMessage(0, "Unable to read manifest");

		return false;
	}

	CInstallationVersionInfo versionInfo;
	versionInfo.SetVersionFromString(manifest.GetVersion().remove("V").trimmed());
	m_installationInfo.SetProductVersion(versionInfo);

	QUrl releaseUrl;
	QString downloadPath = manifest.GetDownloadPath();
	if (!downloadPath.isEmpty()){
		releaseUrl.setHost(m_downloadLinkAttrPtr->GetValue().split(':').first());
		releaseUrl.setPort(m_downloadLinkAttrPtr->GetValue().split(':').last().toUInt());
		releaseUrl.setPath('/' + downloadPrefix + '/' + downloadPath);
	}
	else {
		releaseUrl = QUrl(manifest.GetDownloadLink());
	}

	m_installationInfo.SetReleaseDownloadLink(releaseUrl.toString());
	m_installationInfo.SetInstallationHashType(manifest.GetHashType());
	m_installationInfo.SetInstallationHashValue(manifest.GetHash());

	return true;
}


} // namespace imtupdate

