// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
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


