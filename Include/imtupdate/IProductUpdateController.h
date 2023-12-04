#pragma once


// ACF includes
#include <istd/IPolymorphic.h>
#include <iprm/IParamsSet.h>

// anoter includes
#include<ibase/IProgressManager.h>

// imtupdate includes
#include<imtupdate/IDownloader.h>


namespace imtupdate
{


class IProductUpdateController: virtual public istd::IPolymorphic
{
public:
	/*!
	 \brief CheckForNewVersion
	 \param releaseVersion - [output] - binary encoded available version
	 */
	virtual bool CheckForNewVersion(quint32* releaseVersion = nullptr) const = 0;
	virtual bool InstallVersion(qint32 version = -1) = 0;
	virtual IDownloader::DownloadStatus DownloadNewVersion(ibase::IProgressManager* progressManagerPtr = nullptr) = 0;
	virtual bool FinishAppInstallation(iprm::IParamsSet& params, bool* isRestartRequired = nullptr) = 0;
	virtual bool DelayUpdate(qint32 version = -1) = 0;
	virtual bool HasDelayedUpdate() const = 0;
	virtual bool InstallDelayedUpdate() = 0;
};


} // namespace imtupdate

