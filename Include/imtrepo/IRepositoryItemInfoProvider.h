#pragma once


// Qt includes
#include <QtCore/QSet>
#include <QtCore/QString>

// ACF includes
#include <istd/IChangeable.h>

// ImtCore includes
#include <imtbase/ICollectionInfo.h>


namespace imtrepo
{


class IRepositoryItemInfo
{
	enum RepositoryFileType
	{
		RFT_INFO = 0,
		RFT_DATA,
		RFT_DATA_METAINFO
	};

	typedef QSet<RepositoryFileType> RepositoryFileTypes;

	virtual RepositoryFileTypes GetRepositoryItemFileTypes() const = 0;
	virtual QString GetRepositoryItemFilePath(RepositoryFileType fileId) const = 0;
};


class IRepositoryItemInfoProvider: virtual public istd::IChangeable
{
public:
	virtual const imtbase::ICollectionInfo& GetRepositoryItems() = 0;

	virtual IRepositoryItemInfo GetRepositoryItemInfo(const QByteArray& itemId) const = 0;
};


} // namespace imtrepo


