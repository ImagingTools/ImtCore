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
		if (text == ""){
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

				MainDocumentManager.openDocument(resultItem.m_id, item.m_contextId, item.m_contextTypeId);
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


