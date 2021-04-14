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
		/**
			File with the strutural information about repository item.
		*/
		RFT_INFO = 0,

		/**
			File with the object content.
		*/
		RFT_DATA,

		/**
			Object content meta-info file.
		*/
		RFT_DATA_METAINFO
	};

	typedef QSet<RepositoryFileType> RepositoryFileTypes;

	virtual RepositoryFileTypes GetRepositoryItemFileTypes() const = 0;
	virtual QString GetRepositoryItemFilePath(RepositoryFileType fileType) const = 0;
};


} // namespace imtrepo


