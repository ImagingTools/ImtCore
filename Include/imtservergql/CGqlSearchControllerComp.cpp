// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtservergql/CGqlSearchControllerComp.h>
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/Search.h>


// ACF includes
#include <istd/TDelPtr.h>


namespace imtservergql
{


// protected methods

// reimplemented (sdl::V1_0::imtbase::CSearchGqlHandlerCompBase)

sdl::V1_0::imtbase::CSearchResults CGqlSearchControllerComp::OnSearch(
			const sdl::V1_0::imtbase::CSearchGqlRequest& searchRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	sdl::V1_0::imtbase::CSearchResults response;

	sdl::V1_0::imtbase::SearchRequestArguments arguments = searchRequest.GetRequestedArguments();
	if (!arguments.input.has_value()){
		Q_ASSERT(false);
		return response;
	}

	QString text;
	if (arguments.input->text){
		text = *arguments.input->text;
	}

	imtsdl::TElementList<sdl::V1_0::imtbase::CSearchResult> searchResultList;

	for (int i = 0; i < m_searchControllersCompPtr.GetCount(); i++){
		imtbase::ISearchController* searchControllerPtr = m_searchControllersCompPtr[i];
		if (searchControllerPtr != nullptr){
			istd::TDelPtr<const imtbase::ISearchResults> searchResultsPtr(searchControllerPtr->Search(text));
			if (searchResultsPtr.IsValid()){
				int count = searchResultsPtr->GetSearchResultsCount();
				if (count > 0){
					sdl::V1_0::imtbase::CSearchResult searchRepresentation;

					QByteArray controllerId = searchControllerPtr->GetControllerId();
					searchRepresentation.id = controllerId;

					QString controllerName = searchControllerPtr->GetControllerName();
					if (controllerName.isEmpty()){
						controllerName = controllerId;
					}
					searchRepresentation.name = controllerName;

					imtsdl::TElementList<sdl::V1_0::imtbase::CResultItem> resultItemList;

					for (int resultIndex = 0; resultIndex < count; ++resultIndex){
						imtbase::ISearchResults::SearchResult searchResult = searchResultsPtr->GetSearchResult(resultIndex);
						if (searchResult.resultName.isEmpty()){
							searchResult.resultName = QT_TR_NOOP("Unnamed result");
						}

						sdl::V1_0::imtbase::CResultItem itemRepresentation;
						itemRepresentation.resultName = searchResult.resultName;
						itemRepresentation.contextId = searchResult.contextId;
						itemRepresentation.contextTypeId = searchResult.contextTypeId;
						itemRepresentation.resultDescription = searchResult.resultDescription;
						
						sdl::V1_0::imtbase::CObjectLink objectLink;
						sdl::V1_0::imtbase::CUrlParam urlParam;
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

	response.searchResults = searchResultList;

	return response;
}


} // namespace imtservergql


