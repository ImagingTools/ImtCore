// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtbase/ISearchResults.h>


namespace imtbase
{


class CSearchResults: virtual public imtbase::ISearchResults
{
public:
	void AddSearchResult(const imtbase::ISearchResults::SearchResult& searchResult);

	// reimplemented (ISearchResults)
	virtual int GetSearchResultsCount() const override;
	virtual imtbase::ISearchResults::SearchResult GetSearchResult(int index) const override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (iser::IChangeable)
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual bool IsEqual(const IChangeable& object) const override;
	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

private:
	SearchResultList m_searchResults;
};


} // namespace imtbase


