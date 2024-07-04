#pragma once


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
		QString resultName;
		QString resultDescription;

		bool operator == (const SearchResult& other) const
		{
			return (contextId == other.contextId) &&
					(resultName == other.resultName) &&
					(resultDescription == other.resultDescription);
		}
	};

	typedef QList<SearchResult> SearchResultList;

	virtual int GetSearchResultsCount() const = 0;
	virtual SearchResult GetSearchResult(int index) const = 0;
};


} // namespace imtbase


