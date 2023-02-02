#pragma once


// Gmg includes
#include <imtcom/IFileTransfer.h>


namespace imthttp
{


class CHttpFileTransfer: virtual public imtcom::IFileTransfer
{
public:
	virtual bool UploadFile(const QString& filePath, const QUrl& url) const override;
	virtual bool DownloadFile(const QString& filePath, const QUrl& url) const override;
};


} // namespace imthttp


