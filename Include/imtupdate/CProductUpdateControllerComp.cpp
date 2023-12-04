#include<imtupdate/CProductUpdateControllerComp.h>

// OS API includes
#ifdef Q_OS_WINDOWS
#include <tchar.h>
#include <stdio.h>
#include <windows.h>
#include <TlHelp32.h>
#endif

#ifdef Q_OS_LINUX
#include <unistd.h>
#endif

// Qt includes
#include<QtCore/QFileInfo>
#include<QtCore/QCryptographicHash>
#include<QtCore/QUrl>
#include<QtCore/QDir>
#include<QtCore/QtDebug>
#include<QtCore/QProcess>
#include<QtGui/QDesktopServices>

#ifdef Q_OS_ANDROID
#include <QtAndroidExtras/QtAndroid>
#include <QtAndroidExtras/QAndroidJniEnvironment>
#include <QtAndroidExtras/QAndroidJniObject>
#include <QtAndroidExtras/QAndroidIntent>
#include <imtupdate/CJniAdapter.h>
#endif

// ACF includes
#include <iprm/INameParam.h>
#include <iprm/ITextParam.h>
#include <istd/CSystem.h>

// imtupdate includes
#include <imtupdate/imtupdate.h>


namespace imtupdate
{


CProductUpdateControllerComp::CProductUpdateControllerComp()
	: BaseClass(),
	  m_isDownloadInProgress(false)
{
}


bool CProductUpdateControllerComp::CheckForNewVersion(quint32* releaseVersion) const
{
	QDir dir;
	qDebug() << __FILE__ << __LINE__ << "Creating dir" << GetPathToInstall();
	qDebug() << __FILE__ << __LINE__ << dir.mkpath(GetPathToInstall());

	qDebug() << __FILE__ << __LINE__ << GetPathToInstall() << "IS" << dir.exists(GetPathToInstall());
	Q_ASSERT(QDir().exists(GetPathToInstall()));

	const IInstallationInfo* instalationInfoPtr = m_installationInfoProviderCompPtr->GetInstallationInfo();
	if (instalationInfoPtr != nullptr){
		quint32 availableReleaseVersion = 0;
		quint32 currentVersion = 0;
		instalationInfoPtr->GetProductVersion().GetVersionNumber(*m_versionIdAttrPtr, availableReleaseVersion);
		m_currentApplicationVersionInfoCompPtr->GetVersionNumber(*m_versionIdAttrPtr, currentVersion);
		if(releaseVersion != nullptr){
			*releaseVersion = availableReleaseVersion;
		}

		if (availableReleaseVersion > currentVersion){
			SendInfoMessage(0,
							QString("Product version '%1' is available ")
								.arg(instalationInfoPtr->GetProductVersion().GetEncodedVersionName(*m_versionIdAttrPtr, availableReleaseVersion)));

			return true;
		}
	}
	else{
		if (IsVerboseEnabled()){
			SendErrorMessage(0, "An unexpected error has been occurred during the checking new version");
		}
	}

	return false;
}


bool CProductUpdateControllerComp::InstallVersion(qint32 /*version*/)
{
// copy and open the app
#if defined Q_OS_ANDROID

	QFileInfo fileInfo(m_downloadedFilePath);
	Q_ASSERT(fileInfo.exists());

	if(fileInfo.exists()){
			QString authory;
			if (m_additionalParamsInfoCompPtr.IsValid()){
				const iprm::ITextParam* parameterValue = dynamic_cast<const iprm::ITextParam*> (m_additionalParamsInfoCompPtr->GetParameter("Authory"));
				if (parameterValue != nullptr){
					authory = parameterValue->GetText();
				}
			}

			return CJniAdapter::InstallNewApp(fileInfo.absoluteFilePath(), authory);
	}

	return false;

#elif defined Q_OS_WIN
	QFileInfo newAppFileInfo(m_downloadedFilePath);
	newAppFileInfo.setFile(QCoreApplication::applicationDirPath() + '/' + newAppFileInfo.baseName() + ".new.exe");
	QFile::remove(newAppFileInfo.absoluteFilePath());
	if(QFile::rename(m_downloadedFilePath, newAppFileInfo.absoluteFilePath())){
		QStringList environment;
		for(char **current = _environ; *current; current++)
		{
			environment << *current;
		}
		QProcess process;
		process.setEnvironment(environment);
		if (!process.startDetached(qApp->arguments()[0], QStringList() << "--terminate-process-pid" << QString::number(qApp->applicationPid()))){
			SendErrorMessage(0, QString("Unable to open file '%1' to begin the update process").arg(newAppFileInfo.absoluteFilePath()));

			I_CRITICAL();
		}
	}
	else{
		SendErrorMessage(0, QString("Unable to copy file '%1' to begin the update process").arg(newAppFileInfo.absoluteFilePath()));

		I_CRITICAL();
	}
#elif defined Q_OS_MAC
	I_CRITICAL();
#elif defined Q_OS_LINUX
	I_CRITICAL();
#endif

	return false;
}


IDownloader::DownloadStatus CProductUpdateControllerComp::DownloadNewVersion(ibase::IProgressManager* progressManagerPtr)
{
	IDownloader::DownloadStatus downloadResult = IDownloader::DS_FAILED;
	m_isDownloadInProgress = true;
	const IInstallationInfo* instalationInfoPtr = m_installationInfoProviderCompPtr->GetInstallationInfo();
	if (instalationInfoPtr == nullptr){
		m_isDownloadInProgress = false;
		return downloadResult;
	}

	QString releaseDownloadLink = instalationInfoPtr->GetReleaseDownloadLink();
	SendInfoMessage(0, QString("Starting download new version DownloadLink = %1").arg(releaseDownloadLink), "UpdateController");

	m_downloadedFilePath = GetPathToInstall() + '/' + releaseDownloadLink.split('/').last();
	downloadResult = m_downloaderCompPtr->DownloadFile(releaseDownloadLink, m_downloadedFilePath, true, progressManagerPtr);

	/// \todo use hashGenerator
	QString installationHashType = instalationInfoPtr->GetInstallationHashType();
	Q_ASSERT(installationHashType.compare("MD5", Qt::CaseInsensitive) == 0);

	QString installationHash = instalationInfoPtr->GetInstallationHashValue();
	QString downloadedHash = GetHashForFile(m_downloadedFilePath);

	if(installationHash.compare(downloadedHash, Qt::CaseInsensitive) != 0){
		downloadResult = IDownloader::DS_FAILED;
		SendErrorMessage(0, "Getting new version error. Invalid hash. Removing corrupted file...");
		QFile::remove(m_downloadedFilePath);
	}

	m_isDownloadInProgress = false;
	return downloadResult;
}


// protected methods

QByteArray CProductUpdateControllerComp::GetHashForFile(const QString &filePath) const
{
	QByteArray retVal;
	QFile file(filePath);
	if (file.open(QFile::ReadOnly)){
		QCryptographicHash hashtest(QCryptographicHash::Md5);
		if (hashtest.addData(&file)){
			retVal = hashtest.result().toHex();
		}
	}
	return retVal;
}


// reimplemented (icomp::CComponentBase)

void CProductUpdateControllerComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

#ifdef Q_OS_ANDROID
	CJniAdapter::EnsureApkInstallIsPermitted();
#endif
}


bool CProductUpdateControllerComp::FinishAppInstallation(iprm::IParamsSet& params, bool* isRestartRequired)
{	
	if(HasDelayedUpdate()){
		InstallDelayedUpdate();
	}
#if defined Q_OS_ANDROID
	if(isRestartRequired != nullptr){
		*isRestartRequired = false;
	}
	bool retVal = QDir(GetPathToInstall()).removeRecursively();
	if(!retVal){
		SendErrorMessage(0, QString("Unable te remove dir '%1'").arg(GetPathToInstall()));
		I_CRITICAL();
	}
	return retVal;

#elif defined Q_OS_WIN
	bool retVal = FinishAppInstallationForWindows(params);
	if(retVal && isRestartRequired != nullptr){
		*isRestartRequired = true;
	}
	return retVal;

#elif defined Q_OS_MAC
	I_CRITICAL();

#elif defined Q_OS_LINUX
	I_CRITICAL();
#endif

	I_CRITICAL();
	return false;
}


bool CProductUpdateControllerComp::DelayUpdate(qint32 /*version*/)
{
	bool retVal = false;

	if (m_isDownloadInProgress){
		I_CRITICAL();
		return retVal;
	}

#ifdef Q_OS_ANDROID
	retVal = istd::CSystem::CopyDirectory(GetPathToInstall(), GetPathToInstall() + QStringLiteral("_DELAYED"));
#elif defined Q_OS_WIN
	QFileInfo newAppFileInfo(m_downloadedFilePath);
	newAppFileInfo.setFile(QCoreApplication::applicationDirPath() + '/' + newAppFileInfo.baseName() + ".new.exe");
	QFile::remove(newAppFileInfo.absoluteFilePath());
	retVal = QFile::copy(m_downloadedFilePath, newAppFileInfo.absoluteFilePath());
#else
	I_CRITICAL();
#endif
	return retVal;
}


bool CProductUpdateControllerComp::HasDelayedUpdate() const
{
	bool retVal = false;

#ifdef Q_OS_ANDROID
	QDir delayedUpdatesDir(GetPathToInstall() + QStringLiteral("_DELAYED"));
	retVal = delayedUpdatesDir.exists();
	if (retVal){
		QFileInfoList delayedUpdatesDirEntryList = delayedUpdatesDir.entryInfoList({"*.apk"}, QDir::Files);
		Q_ASSERT(delayedUpdatesDirEntryList.count() < 2);
		retVal = delayedUpdatesDirEntryList.count() > 0;
	}
#elif defined Q_OS_WIN
	QFileInfo newAppFileInfo(m_downloadedFilePath);
	newAppFileInfo.setFile(QCoreApplication::applicationDirPath() + '/' + newAppFileInfo.baseName() + ".new.exe");
	retVal = QFile::exists(newAppFileInfo.absoluteFilePath());
#else
	I_CRITICAL();
#endif

	return retVal;
}


bool CProductUpdateControllerComp::InstallDelayedUpdate()
{
	bool retVal = false;

	if (!HasDelayedUpdate() || m_isDownloadInProgress){
		I_CRITICAL();
		return retVal;
	}

#ifdef Q_OS_ANDROID
	QDir delayedUpdatesDir(GetPathToInstall() + QStringLiteral("_DELAYED"));
	retVal = delayedUpdatesDir.exists();
	if (retVal){
		QFileInfoList delayedUpdatesDirEntryList = delayedUpdatesDir.entryInfoList({"*.apk"}, QDir::Files);
		Q_ASSERT(delayedUpdatesDirEntryList.count() < 2);
		retVal = delayedUpdatesDirEntryList.count() > 0;
		retVal = CJniAdapter::EnsureApkInstallIsPermitted();
		if (retVal){
			QFileInfo apkFileInfo(delayedUpdatesDirEntryList.first());
			QString apkVersion = CJniAdapter::CheckApkVer(apkFileInfo.absoluteFilePath());
			QString thisPackageVersion = CJniAdapter::GetThisPackageVersion();
			retVal = apkVersion != thisPackageVersion;
			if(retVal){
				QString authory;
				if (m_additionalParamsInfoCompPtr.IsValid()){
					const iprm::ITextParam* parameterValue = dynamic_cast<const iprm::ITextParam*> (m_additionalParamsInfoCompPtr->GetParameter("Authory"));
					if (parameterValue != nullptr){
						authory = parameterValue->GetText();
					}
				}
				retVal = CJniAdapter::InstallNewApp(apkFileInfo.absoluteFilePath(), authory);
			}
		}
	}
#elif defined Q_OS_WIN
	QStringList environment;
	for(char **current = _environ; *current; current++){
		environment << *current;
	}
	QProcess process;
	process.setEnvironment(environment);
	if (!process.startDetached(qApp->arguments()[0], QStringList() << "--terminate-process-pid" << QString::number(qApp->applicationPid()))){
		SendErrorMessage(0, QString("Unable to open file '%1' to begin the update process").arg(qApp->arguments()[0]));

		I_CRITICAL();
	}
#else
	I_CRITICAL();
#endif

	return retVal;
}


bool CProductUpdateControllerComp::FinishAppInstallationForWindows(iprm::IParamsSet& params) const
{
/// \todo change debug to verbose messages
#ifdef Q_OS_WIN
	QStringList appArgs = qApp->arguments();
	iprm::IParamsSet::Ids paramIds = params.GetParamIds();

	std::function<QString(const QByteArray&)> getArgumentValue = [&params, &paramIds](const QByteArray& argId){
		if(paramIds.contains(argId)){
			const iprm::INameParam* parameterValue = dynamic_cast<const iprm::INameParam*> (params.GetParameter(argId));
			if (parameterValue != nullptr){
				return parameterValue->GetName();
			}
		}
		return QString();
	};

	if(paramIds.contains("--terminate-process-pid"))
		_winTerminateProcess(getArgumentValue("--terminate-process-pid").toLongLong());

	/// refresh update
	if(appArgs[0].endsWith(".new.exe") && !appArgs.contains("--no-refresh-update"))
	{
		QString newAppFileName = appArgs[0];
		newAppFileName.remove(newAppFileName.lastIndexOf(".new.exe"), 8);
		newAppFileName.append(".exe");

		if(appArgs.contains("--pid"))
		{
			qDebug() << "FinishAppInstallationForWindows" << "terminating " << getArgumentValue("--pid").toLongLong();
			while(1)
			{
				bool retmRes =  _winTerminateProcess(getArgumentValue("--pid").toLongLong());
				qDebug() << "FinishAppInstallationForWindows" << retmRes;
				if(retmRes) break;
				else ::Sleep(1000);
			}
		}

		_winTerminateProcess(newAppFileName.split(QDir::separator()).last());

		int renameAttempts = 0, renameAttempts_max = 3;
		while(1)
		{
			QFile::remove(newAppFileName + ".old");
			bool renRes = QFile::rename(newAppFileName, newAppFileName + ".old");
			qDebug() << "FinishAppInstallationForWindows" << "renaming " << newAppFileName << newAppFileName + ".old" << renRes;
			if(renRes) break;
			else ::Sleep(1000);
			if(++renameAttempts > renameAttempts_max) break;
		}

		QFile currentFile(appArgs[0]);
		currentFile.open(QFile::ReadOnly);
		QFile newAppFile(newAppFileName);
		newAppFile.open(QFile::WriteOnly);
		newAppFile.write(currentFile.readAll());
		currentFile.close();
		newAppFile.close();
		QFileInfo newApp(newAppFileName);
		qDebug() << "FinishAppInstallationForWindows" << newApp.exists() << newApp.isExecutable();
		if(newApp.exists() && newApp.isExecutable())
		{
			QStringList environment;
			for(char **current = _environ; *current; current++)
			{
				environment << *current;
			}
			QProcess *newP = new QProcess;
			newP->setEnvironment(environment);
			appArgs << ("--no-refresh-update");
			appArgs << "--pid" << QString::number(qApp->applicationPid());
			if( newP->startDetached(newApp.absoluteFilePath(), appArgs))
			{
				qDebug() << "FinishAppInstallationForWindows" << "started exiting";
				::exit(0);
			}
			else
			{
				newP->deleteLater();
			}
		}
	}
	if(!appArgs[0].endsWith(".new.exe") && appArgs.contains("--no-refresh-update"))
	{
		QString newAppFileName = appArgs[0];
		newAppFileName.remove(newAppFileName.lastIndexOf(".exe"), 4);
		newAppFileName.append(".new.exe");
		qDebug() << "FinishAppInstallationForWindows" << "removing" << newAppFileName <<
					QFile::remove(newAppFileName);
	}

	QString newAppFileName = qApp->applicationFilePath();
	newAppFileName.remove(newAppFileName.lastIndexOf(".exe"), 4);
	newAppFileName.append(".new.exe");
	QFileInfo newApp(newAppFileName);
	qDebug() << "FinishAppInstallationForWindows" << newAppFileName << newApp.exists() << newApp.isExecutable() << !appArgs[0].endsWith(".new.exe");

	if(newApp.exists() && newApp.isExecutable() && !appArgs[0].endsWith(".new.exe"))
	{
		QStringList environment;
		for(char **current = _environ; *current; current++)
		{
			environment << *current;
		}
		appArgs << "--no-check-update";
		appArgs << "--pid" << QString::number(qApp->applicationPid());
		QProcess *newP = new QProcess;
		newP->setEnvironment(environment);
		if( newP->startDetached(newApp.absoluteFilePath(), appArgs))
		{
			qDebug() << "FinishAppInstallationForWindows" << "started";
			::exit(0);
		}
		else
		{
			QFile(newApp.absoluteFilePath()).remove();
			newP->deleteLater();
		}
	}
#else
	// this method should not be called from not Windows OS
	I_CRITICAL();
#endif
	return false;
}


qlonglong CProductUpdateControllerComp::_winProcessPIDByName(const QString& pName) const
{
	int retval = -1;
#ifdef Q_OS_WINDOWS
	HANDLE hSnap = NULL;
	PROCESSENTRY32W pe32;
	pe32.dwSize = sizeof( PROCESSENTRY32 );
	hSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnap!=NULL)
	{
		if (!Process32FirstW(hSnap, &pe32)) return retval;
		while (Process32NextW(hSnap, &pe32))
		{
			if(QString::fromWCharArray(pe32.szExeFile) == pName)
			{
				retval = pe32.th32ProcessID;
				break;
			}
		}
	}
	::CloseHandle(hSnap);
	return FALSE;
#else
	qCritical() << "THIS METHOD HAS AN EFFECT IN WINDOWS OS ONLY!";
#endif
	return retval;
}


bool CProductUpdateControllerComp::_winTerminateProcess(const QString& pName) const
{
	int retval = 0;
#ifdef Q_OS_WINDOWS
	if(!_winProcessIsRunning(pName)) return true;
	HANDLE kill = ::OpenProcess(PROCESS_ALL_ACCESS, TRUE, _winProcessPIDByName(pName));
	DWORD fdwExit = 0;
	::GetExitCodeProcess(kill, &fdwExit);
	retval = ::TerminateProcess(kill, fdwExit);
	::CloseHandle(kill);
#else
	qCritical() << "THIS METHOD HAS AN EFFECT IN WINDOWS OS ONLY!";
#endif
	if(retval && _winProcessIsRunning(pName)) return _winTerminateProcess(pName);
	return retval;
}


bool CProductUpdateControllerComp::_winTerminateProcess(const qlonglong& pID) const
{
	int retval = 0;
#ifdef Q_OS_WINDOWS
	if(!_winProcessIsRunning(pID)) return true;
	HANDLE kill = ::OpenProcess(PROCESS_ALL_ACCESS, TRUE, pID);
	DWORD fdwExit = 0;
	::GetExitCodeProcess(kill, &fdwExit);
	retval = ::TerminateProcess(kill, fdwExit);
	::CloseHandle(kill);
#else
	qCritical() << "THIS METHOD HAS AN EFFECT IN WINDOWS OS ONLY!";
#endif
	return retval;
}


bool CProductUpdateControllerComp::_winProcessIsRunning(const qlonglong& pID) const
{
	int retval = 0;
	if(pID < 0) return retval;
#ifdef Q_OS_WINDOWS
	HANDLE hSnap = NULL;
	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof( PROCESSENTRY32 );
	hSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnap!=NULL)
	{
		if (Process32First(hSnap, &pe32) && pe32.th32ProcessID == pID)
			return true;
		while (Process32Next(hSnap, &pe32))
		{
			if(pe32.th32ProcessID == pID) return TRUE;
		}
	}
	::CloseHandle(hSnap);
	return FALSE;
#else
	qCritical() << "THIS METHOD HAS AN EFFECT IN WINDOWS OS ONLY!";
#endif
	return retval;
}


bool CProductUpdateControllerComp::_winProcessIsRunning(const QString& pName) const
{
	int retval = 0;
#ifdef Q_OS_WINDOWS
	HANDLE hSnap = NULL;
	PROCESSENTRY32W pe32;
	pe32.dwSize = sizeof( PROCESSENTRY32 );
	hSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnap!=NULL)
	{
		if (Process32FirstW(hSnap, &pe32) && QString::fromWCharArray(pe32.szExeFile) == pName)
			return true;
		while (Process32NextW(hSnap, &pe32))
		{
			if(QString::fromWCharArray(pe32.szExeFile) == pName) return TRUE;
		}
	}
	::CloseHandle(hSnap);
	return FALSE;
#else
	qCritical() << "THIS METHOD HAS AN EFFECT IN WINDOWS OS ONLY!";
#endif
	return retval;
}


}//namespace imtupdate
