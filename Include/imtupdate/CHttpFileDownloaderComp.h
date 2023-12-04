#pragma once


// ACF includes
#include<ilog/TLoggerCompWrap.h>

// imtupdate includes
#include<imtupdate/IDownloader.h>


namespace imtupdate
{


class CHttpFileDownloaderComp: public ilog::CLoggerComponentBase,
		virtual public IDownloader
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CHttpFileDownloaderComp);
		I_REGISTER_INTERFACE(IDownloader);
	I_END_COMPONENT;

	// reimplemented (IDownloader)
	virtual DownloadStatus DownloadFile(
				const QString& remoteLink,
				const QString& localFilePath,
				bool contunieDownload = false,
				ibase::IProgressManager* progressManagerPtr = nullptr) const override;
};


} // namespace imtupdate

