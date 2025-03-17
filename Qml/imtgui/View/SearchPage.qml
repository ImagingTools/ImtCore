import QtQuick 2.12
import Acf 1.0
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
	
	function createAllTab(){
		let count = root.model.getItemsCount();
		if (count > 0){
			root.model.insertNewItem(0)
			root.model.setData("Id", "All");
			root.model.setData("Name", qsTr("All"));
			
			let allItemsModel = root.model.addTreeModel("Items");
			
			for (let i = 1; i < count + 1; i++){
				let tabId = root.model.getData("Id", i);
				let itemsModel = root.model.getData("Items", i);
				for (let j = 0; j < itemsModel.getItemsCount(); j++){
					let id = itemsModel.getData("Id", j);
					let typeId = itemsModel.getData("TypeId", j);
					let name = itemsModel.getData("Name", j);
					let description = itemsModel.getData("Description", j);
					
					let index = allItemsModel.insertNewItem();
					allItemsModel.setData("Id", id, index);
					allItemsModel.setData("TabId", tabId, index);
					allItemsModel.setData("TypeId", typeId, index);
					allItemsModel.setData("Name", name, index);
					allItemsModel.setData("Description", description, index);
				}
			}
		}
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


