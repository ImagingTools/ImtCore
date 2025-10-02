import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtguigql 1.0
import imtcontrols 1.0
import imtdocgui 1.0
import imtbaseSearchSdl 1.0

Item {
	id: root;

	Component.onCompleted: {
		Events.subscribeEvent("GlobalSearchActivated", root.updateSearch)
	}
	
	Component.onDestruction: {
		Events.unSubscribeEvent("GlobalSearchActivated", root.updateSearch)
	}
	
	function updateSearch(text){
		if (text === ""){
			searchResultsView.searchResults = null;
			return;
		}

		searchInput.m_text = text
		searchRequestSender.send(searchInput)
	}

	SearchResultsView {
		id: searchResultsView;
		anchors.fill: parent;
		
		onItemDoubleClicked: {
			if (searchResults && tabIndex >= 0 && itemIndex >= 0){
				let resultItem = searchResults.m_searchResults.get(tabIndex).item;
				if (!resultItem){
					return;
				}

				let item = resultItem.m_items.get(itemIndex).item;
				if (!item){
					return;
				}

				let objectLink = item.m_objectLink
				if (!objectLink){
					console.error("Unable to navigate to item. Error: Object Link is invalid")
					return
				}

				let url = objectLink.m_url
				if (!url){
					console.error("Unable to navigate to item. Error: URL is invalid")
					return
				}
				
				let params = {}
				params["textFilter"] = searchInput.m_text
				params["resultName"] = item.m_resultName
				params["resultDescription"] = item.m_resultDescription
				params["contextId"] = item.m_contextId
				params["contextTypeId"] = item.m_contextTypeId
				NavigationController.navigate(url.m_path, params)
			}
		}
	}
	
	SearchInput {
		id: searchInput;
		m_text: '';
	}
	
	GqlSdlRequestSender {
		id: searchRequestSender;
		gqlCommandId: ImtbaseSearchSdlCommandIds.s_search;
		
		sdlObjectComp: Component {
			SearchResults {
				onFinished: {
					searchResultsView.searchResults = this;
				}
			}
		}
		
		onStateChanged: {
			if (state === "Loading"){
				searchResultsView.startLoading()
			}
			else{
				searchResultsView.stopLoading()
			}
		}
	}
}


