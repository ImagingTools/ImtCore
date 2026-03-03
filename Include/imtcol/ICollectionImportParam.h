// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <iser/ISerializable.h>

// Qt includes
#include <QtCore/QString>
#include <QtCore/QByteArray>


namespace imtcol
{


class ICollectionImportParam: virtual public iser::ISerializable
{
public:
	struct FileImportInfo
	{
		QString filePath;
		QByteArray objectTypeId;
		QByteArray proposedId;
	};

	virtual QByteArray GetCollectionId() const = 0;
	virtual bool SetCollectionId(const QByteArray& collectionId) = 0;

	virtual int GetFileCount() const = 0;
	virtual FileImportInfo* GetFileImportInfo(int index) const = 0;
	virtual bool InsertFileImportInfo(const FileImportInfo& info, int index = -1) = 0;
	virtual bool RemoveFileImportInfo(int index) = 0;
};


} // namespace imtcol


