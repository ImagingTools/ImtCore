#include <imtservergql/CGqlSearchControllerComp.h>


// ACF includes
#include <istd/TDelPtr.h>


namespace imtservergql
{


// protected methods

// reimplemented (sdl::imtbase::Search::CGraphQlHandlerCompBase)

sdl::imtbase::Search::CSearchResults CGqlSearchControllerComp::OnSearch(
			const sdl::imtbase::Search::CSearchGqlRequest& searchRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	sdl::imtbase::Search::CSearchResults response;

	sdl::imtbase::Search::SearchRequestArguments arguments = searchRequest.GetRequestedArguments();
	if (!arguments.input.Version_1_0.has_value()){
		Q_ASSERT(false);
		return response;
	}

	response.Version_1_0.emplace();

	QString text;
	if (arguments.input.Version_1_0->text){
		text = *arguments.input.Version_1_0->text;
	}

	QList<sdl::imtbase::Search::CSearchResult::V1_0> searchResultList;

	for (int i = 0; i < m_searchControllersCompPtr.GetCount(); i++){
		imtbase::ISearchController* searchControllerPtr = m_searchControllersCompPtr[i];
		if (searchControllerPtr != nullptr){
			istd::TDelPtr<const imtbase::ISearchResults> searchResultsPtr(searchControllerPtr->Search(text));
			if (searchResultsPtr.IsValid()){
				int count = searchResultsPtr->GetSearchResultsCount();
				if (count > 0){
					sdl::imtbase::Search::CSearchResult::V1_0 searchRepresentation;
					searchRepresentation.id = m_optionListCompPtr->GetOptionId(i);
					searchRepresentation.name = m_optionListCompPtr->GetOptionName(i);

					QList<sdl::imtbase::Search::CResultItem::V1_0> resultItemList;

					for (int resultIndex = 0; resultIndex < count; ++resultIndex){
						imtbase::ISearchResults::SearchResult searchResult = searchResultsPtr->GetSearchResult(resultIndex);
						if (searchResult.resultName.isEmpty()){
							searchResult.resultName = QT_TR_NOOP("Unnamed result");
						}

						sdl::imtbase::Search::CResultItem::V1_0 itemRepresentation;
						itemRepresentation.resultName = searchResult.resultName;
						itemRepresentation.contextId = searchResult.contextId;
						itemRepresentation.contextTypeId = searchResult.contextTypeId;
						itemRepresentation.resultDescription = searchResult.resultDescription;

						resultItemList << itemRepresentation;
					}

					searchRepresentation.items = resultItemList;

					searchResultList << searchRepresentation;
				}
			}
		}
	}

	response.Version_1_0->searchResults = searchResultList;

	return response;
}


} // namespace imtservergql


