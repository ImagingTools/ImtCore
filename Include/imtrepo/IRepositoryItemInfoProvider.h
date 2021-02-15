#pragma once


// Qt includes
#include <QtCore/QList>
#include <QtCore/QSet>
#include <QtCore/QString>

// ACF includes
#include <istd/IChangeable.h>

// ImtCore
#include <imtbase/ICollectionInfo.h>


namespace imtrepo
{


class IRepositoryItemInfoProvider: virtual public istd::IChangeable
{
public:
	enum FileId
	{
		FI_ITEM_INFO = 0,
		FI_DATA,
		FI_DATA_METAINFO
	};

	typedef QSet<FileId> FileIds;
	typedef QList<QByteArray> ItemIds;

	virtual ItemIds GetRepositoryItemIds() const = 0;
	virtual FileIds GetRepositoryItemFileIds(const QByteArray& itemId) const = 0;
	virtual QString GetRepositoryItemFilePath(const QByteArray& itemId, FileId fileId) const = 0;
};


} // namespace imtrepo


