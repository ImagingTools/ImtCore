#pragma once


// ACF includes
#include <istd/IPolymorphic.h>


class QUrl;


namespace imtcom
{


class IFileTransfer: virtual public istd::IPolymorphic
{
public:
	virtual bool UploadFile(const QString& filePath, const QUrl& url) const = 0;
	virtual bool DownloadFile(const QString& filePath, const QUrl& url) const = 0;
};


} // namespace imtcom


