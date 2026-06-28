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

	QByteArray specificResultId;
	if (arguments.input->searchResultId && !(*arguments.input->searchResultId).isEmpty()){
		specificResultId = (*arguments.input->searchResultId);
	}

	int reqOffset = 0;
	if (arguments.input->offset){
		reqOffset = *arguments.input->offset;
	}
	int reqCount = 30;
	if (arguments.input->count && *arguments.input->count > 0){
		reqCount = *arguments.input->count;
	}

	imtsdl::TElementList<sdl::V1_0::imtbase::CSearchResult> searchResultList;

	for (int i = 0; i < m_searchControllersCompPtr.GetCount(); i++){
		imtbase::ISearchController* searchControllerPtr = m_searchControllersCompPtr[i];
		if (searchControllerPtr == nullptr){
			continue;
		}

		QByteArray controllerId = searchControllerPtr->GetControllerId();
		if (!specificResultId.isEmpty() && controllerId != specificResultId){
			continue;
		}

		int total = searchControllerPtr->GetMatchCount(text);
		if (total <= 0){
			if (specificResultId.isEmpty()){
				continue;
			}
		}

		sdl::V1_0::imtbase::CSearchResult searchRepresentation;

		searchRepresentation.id = controllerId;

		QString controllerName = searchControllerPtr->GetControllerName();
		if (controllerName.isEmpty()){
			controllerName = controllerId;
		}
		searchRepresentation.name = controllerName;
		searchRepresentation.totalCount = total;

		imtsdl::TElementList<sdl::V1_0::imtbase::CResultItem> resultItemList;

		// When searchResultId is empty, it means show data from the first connected controller.
		// Response includes info for ALL SearchResult (groups), but initially items empty except for the first one
		// which gets the initial page using the provided offset/count.
		// Only load specific group data (with pagination) when GUI switches to that tab.
		bool loadItems = !specificResultId.isEmpty() || searchResultList.isEmpty();
		if (loadItems){
			istd::TDelPtr<const imtbase::ISearchResults> searchResultsPtr(searchControllerPtr->Search(text, reqOffset, reqCount));
			int itemCount = 0;
			if (searchResultsPtr.IsValid()){
				itemCount = searchResultsPtr->GetSearchResultsCount();
			}

			for (int resultIndex = 0; resultIndex < itemCount; ++resultIndex){
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
		}

		searchRepresentation.items = resultItemList;

		searchResultList << searchRepresentation;

		if (!specificResultId.isEmpty()){
			break;
		}
	}

	response.searchResults = searchResultList;

	return response;
}


} // namespace imtservergql


