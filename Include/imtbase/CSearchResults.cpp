#include <imtbase/CSearchResults.h>


// ACF includes
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>


namespace imtbase
{


// public methods

void CSearchResults::AddSearchResult(imtbase::ISearchResults::SearchResult searchResult)
{
	if (!m_searchResults.contains(searchResult)){
		istd::CChangeNotifier changeNotifier(this);

		m_searchResults << searchResult;
	}
}


// reimplemented (ISearchResults)

int CSearchResults::GetSearchResultsCount() const
{
	return m_searchResults.size();
}


imtbase::ISearchResults::SearchResult CSearchResults::GetSearchResult(int index) const
{
	return m_searchResults[index];
}


// reimplemented (iser::ISerializable)

bool CSearchResults::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier changeNotifier(archive.IsStoring() ? nullptr : this);

	bool retVal = true;

	return retVal;
}


} // namespace imtbase


