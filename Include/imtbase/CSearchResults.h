#pragma once


// ImtCore includes
#include <imtbase/ISearchResults.h>


namespace imtbase
{


class CSearchResults: virtual public imtbase::ISearchResults
{
public:
	void AddSearchResult(imtbase::ISearchResults::SearchResult searchResult);

	// reimplemented (ISearchResults)
	virtual int GetSearchResultsCount() const override;
	virtual imtbase::ISearchResults::SearchResult GetSearchResult(int index) const override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

private:
	SearchResultList m_searchResults;
};


} // namespace imtbase


