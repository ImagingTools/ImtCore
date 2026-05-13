// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtbase/CSearchResults.h>


// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>


namespace imtbase
{

// public methods

void CSearchResults::AddSearchResult(const imtbase::ISearchResults::SearchResult& searchResult)
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

	int count = m_searchResults.size();

	if (!archive.IsStoring()){
		m_searchResults.clear();
		count = 0;
	}

	iser::CArchiveTag searchResultsTag("SearchResults", "Search Results", iser::CArchiveTag::TT_MULTIPLE);
	iser::CArchiveTag searchResultTag("SearchResult", "Search Result", iser::CArchiveTag::TT_GROUP, &searchResultsTag);

	retVal = retVal && archive.BeginMultiTag(searchResultsTag, searchResultTag, count);
	for (int index = 0; index < count; index++){
		retVal = retVal && archive.BeginTag(searchResultTag);

		imtbase::ISearchResults::SearchResult searchResult;

		iser::CArchiveTag contextIdTag("ContextId", "Context-ID", iser::CArchiveTag::TT_LEAF);
		retVal = retVal && archive.BeginTag(contextIdTag);
		retVal = retVal && archive.Process(searchResult.contextId);
		retVal = retVal && archive.EndTag(contextIdTag);

		iser::CArchiveTag contextTypeIdTag("ContextTypeId", "Context Type-ID", iser::CArchiveTag::TT_LEAF);
		retVal = retVal && archive.BeginTag(contextTypeIdTag);
		retVal = retVal && archive.Process(searchResult.contextTypeId);
		retVal = retVal && archive.EndTag(contextTypeIdTag);

		iser::CArchiveTag resultNameTag("ResultName", "Result Name", iser::CArchiveTag::TT_LEAF);
		retVal = retVal && archive.BeginTag(resultNameTag);
		retVal = retVal && archive.Process(searchResult.resultName);
		retVal = retVal && archive.EndTag(resultNameTag);

		iser::CArchiveTag resultDescriptionTag("ResultDescription", "Result Description", iser::CArchiveTag::TT_LEAF);
		retVal = retVal && archive.BeginTag(resultDescriptionTag);
		retVal = retVal && archive.Process(searchResult.resultDescription);
		retVal = retVal && archive.EndTag(resultDescriptionTag);

		QString urlStr = searchResult.url.toString();
		iser::CArchiveTag urlTag("Url", "Url", iser::CArchiveTag::TT_LEAF);
		retVal = retVal && archive.BeginTag(urlTag);
		retVal = retVal && archive.Process(urlStr);
		retVal = retVal && archive.EndTag(urlTag);

		if (retVal && !archive.IsStoring()){
			searchResult.url = QUrl(urlStr);
		}

		retVal = retVal && archive.EndTag(searchResultTag);

		if (retVal && !archive.IsStoring()){
			m_searchResults << searchResult;
		}
	}

	retVal = retVal && archive.EndTag(searchResultsTag);

	return retVal;
}


// reimplemented (iser::IChangeable)

bool CSearchResults::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const CSearchResults* sourcePtr = dynamic_cast<const CSearchResults*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier changeNotifier(this);

		m_searchResults = sourcePtr->m_searchResults;

		return true;
	}

	return false;
}


bool CSearchResults::IsEqual(const IChangeable& object) const
{
	bool retVal = true;

	const CSearchResults* sourcePtr = dynamic_cast<const CSearchResults*>(&object);
	if (retVal && sourcePtr != nullptr){
		retVal = retVal && m_searchResults == sourcePtr->m_searchResults;

		return retVal;
	}

	return false;
}


istd::IChangeableUniquePtr CSearchResults::CloneMe(CompatibilityMode mode) const
{
	istd::IChangeableUniquePtr clonePtr(new CSearchResults);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr;
	}

	return nullptr;
}


bool CSearchResults::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier changeNotifier(this);

	m_searchResults.clear();

	return true;
}


} // namespace imtbase


