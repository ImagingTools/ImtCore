#pragma once


// Qt includes
#include <QtCore/QSet>
#include <QtCore/QString>

// ACF includes
#include <istd/IChangeable.h>


namespace imtrepo
{


class IRepositoryItemInfo: virtual public istd::IChangeable
{
public:
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


} // namespace imtrepo


