#pragma once


// Qt includes
#include <QtCore/QUrl>

// ACF includes
#include <iser/ISerializable.h>


namespace imtbase
{


class ISearchResults: virtual public iser::ISerializable
{
public:
	struct SearchResult
	{
		QByteArray contextId;
		QByteArray contextTypeId;
		QString resultName;
		QString resultDescription;
		QUrl url;

		bool operator == (const SearchResult& other) const
		{
			return (contextId == other.contextId) &&
					(contextTypeId == other.contextTypeId) &&
					(resultName == other.resultName) &&
					(url == other.url) &&
					(resultDescription == other.resultDescription);
		}
	};

	typedef QList<SearchResult> SearchResultList;

	virtual int GetSearchResultsCount() const = 0;
	virtual SearchResult GetSearchResult(int index) const = 0;
};


} // namespace imtbase


