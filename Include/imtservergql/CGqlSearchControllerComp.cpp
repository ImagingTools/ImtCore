// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
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

	imtsdl::TElementList<sdl::imtbase::Search::CSearchResult::V1_0> searchResultList;

	for (int i = 0; i < m_searchControllersCompPtr.GetCount(); i++){
		imtbase::ISearchController* searchControllerPtr = m_searchControllersCompPtr[i];
		if (searchControllerPtr != nullptr){
			istd::TDelPtr<const imtbase::ISearchResults> searchResultsPtr(searchControllerPtr->Search(text));
			if (searchResultsPtr.IsValid()){
				int count = searchResultsPtr->GetSearchResultsCount();
				if (count > 0){
					sdl::imtbase::Search::CSearchResult::V1_0 searchRepresentation;

					QByteArray controllerId = searchControllerPtr->GetControllerId();
					searchRepresentation.id = controllerId;

					QString controllerName = searchControllerPtr->GetControllerName();
					if (controllerName.isEmpty()){
						controllerName = controllerId;
					}
					searchRepresentation.name = controllerName;

					imtsdl::TElementList<sdl::imtbase::Search::CResultItem::V1_0> resultItemList;

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
						
						sdl::imtbase::ImtBaseTypes::CObjectLink::V1_0 objectLink;
						sdl::imtbase::ImtBaseTypes::CUrlParam::V1_0 urlParam;
						urlParam.scheme = "applink";
						urlParam.path = searchResult.url.path();
						objectLink.url = urlParam;
						itemRepresentation.objectLink = objectLink;

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


