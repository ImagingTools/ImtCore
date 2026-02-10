// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
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


