#pragma once


// ACF includes
#include<ilog/TLoggerCompWrap.h>
#include<ifile/IFileNameParam.h>
#include<iprm/IParamsSet.h>

// imtupdate includes
#include<imtupdate/IProductUpdateController.h>
#include<imtupdate/IInstallationInfoProvider.h>

namespace imtupdate
{
/**
	The CProductUpdateControllerComp class for updating controllers
 */
class CProductUpdateControllerComp:
		virtual public IProductUpdateController,
		public ilog::CLoggerComponentBase
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CProductUpdateControllerComp);
		I_REGISTER_INTERFACE(IProductUpdateController);

		I_ASSIGN(m_downloaderCompPtr, "ComponentDownloader", "Downloader component", true, "ComponentDownloader");
		I_ASSIGN(m_installationInfoProviderCompPtr, "InstalationInfoProvider", "Provider, which installation info", true, "InstalationInfoProvider");
		I_ASSIGN(m_currentApplicationVersionInfoCompPtr, "CurrentApplicationVersion", "The current application version", true, "IVersionInfo");
		I_ASSIGN(m_versionIdAttrPtr, "ProductVersionId", "Binary ID of the product version", true, 2000);
		I_ASSIGN(m_additionalParamsInfoCompPtr, "AdditionalParameters", "Additional parameters to specify behaviour for the product", true, "AdditionalParameters");
	I_END_COMPONENT;

	CProductUpdateControllerComp();

	// reimplemented (IProductUpdateController)
	virtual bool CheckForNewVersion(quint32* releaseVersion = nullptr) const override;
	virtual bool InstallVersion(qint32 version = -1) override;
	virtual IDownloader::DownloadStatus DownloadNewVersion(ibase::IProgressManager* progressManagerPtr = nullptr) override;
	virtual bool FinishAppInstallation(iprm::IParamsSet& params, bool* isRestartRequired = nullptr) override;
	virtual bool DelayUpdate(qint32 version = -1) override;
	virtual bool HasDelayedUpdate() const override;
	virtual bool InstallDelayedUpdate() override;

protected:
	QByteArray GetHashForFile(const QString &filePath) const;

	virtual bool FinishAppInstallationForWindows(iprm::IParamsSet& params) const;
	/**
		WIN-API wrapped methods
	 */
	qlonglong _winProcessPIDByName(const QString &pName) const;
	bool _winTerminateProcess(const QString &pName) const;
	bool _winTerminateProcess(const qlonglong &pID) const;
	bool _winProcessIsRunning(const qlonglong &pID) const;
	bool _winProcessIsRunning(const QString &pName) const;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

private:
	I_REF(IDownloader, m_downloaderCompPtr);
	I_REF(IInstallationInfoProvider, m_installationInfoProviderCompPtr);
	I_REF(iser::IVersionInfo, m_currentApplicationVersionInfoCompPtr);
	I_REF(iprm::IParamsSet, m_additionalParamsInfoCompPtr);

	I_ATTR(int, m_versionIdAttrPtr);

	mutable QString m_downloadedFilePath;
	bool m_isDownloadInProgress;
};


}//namespace imtupdate
