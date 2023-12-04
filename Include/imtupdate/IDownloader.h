#pragma once

// Acf includes
#include <istd/IPolymorphic.h>

// ibase includes
#include<ibase/IProgressManager.h>


namespace imtupdate
{


class IDownloader: virtual public istd::IPolymorphic
{
public:
	enum DownloadStatus
	{
		DS_OK,
		DS_CANCELED,
		DS_FAILED
	};

	virtual DownloadStatus DownloadFile(
				const QString& remoteLink,
				const QString& localFilePath,
				bool contunieDownload = false,
				ibase::IProgressManager* progressManagerPtr = nullptr) const = 0;
};


} // namespace svnupdate

